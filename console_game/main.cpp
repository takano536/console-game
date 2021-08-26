#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "StlAllocator.h"
#include "FpsSustainer.h"
#include "Renderer.h"
#include "Color.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Goal.h"

constexpr short HORIZONTAL_MARGIN = 20;       // 横方向の空白幅
constexpr short VERTICAL_MARGIN = 5;          // 縦方向の空白幅
constexpr char map_filepath[] = "./map.txt";  // マップデータのファイルパス

int main()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(static_cast<std::vector<Enemy, StlAllocator<Enemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_point = COORD{1, 1};
	COORD player_goal_point = COORD{static_cast<short>(renderer.get_max_height() - VERTICAL_MARGIN - 1), static_cast<short>(renderer.get_max_width() - HORIZONTAL_MARGIN - 1)};

	// map.txt からマップ情報を読み込む
	std::ifstream ifs(map_filepath);
	if (ifs.fail())
	{
		std::cerr << "Failed to open \"map.txt\"..." << std::endl;
		std::cout << "Press escape key to exit" << std::endl;
		while (!GetAsyncKeyState(VK_ESCAPE))
			;
		return -1;
	}
	std::string str;
	short i = 0, j = 0;
	bool is_failed = false;  // ファイル読み込みが失敗したかどうか
	while (getline(ifs, str))
	{
		for (auto c : str)
		{
			if (i > renderer.get_max_height() - VERTICAL_MARGIN || j > renderer.get_max_width() - HORIZONTAL_MARGIN)
				is_failed = true;
			switch (c)
			{
				case 'S':
					player_start_point = COORD{j, i};
					break;
				case 'G':
					player_goal_point = COORD{j, i};
					break;
				case '#':
					walls.push_back(Wall(j, i));
					break;
				case 'E':
					enemys.push_back(Enemy(j, i, true));
					break;
				case 'e':
					enemys.push_back(Enemy(j, i, false));
					break;
				default:
					break;
			}
			j++;
		}
		j = 0;
		i++;
	}
	if (is_failed)
	{
		std::cerr << "Failed to open \"map.txt\"..." << std::endl;
		std::cout << "Press escape key to exit" << std::endl;
		while (!GetAsyncKeyState(VK_ESCAPE))
			;
		return -1;
	}

	// vector のメモリ節約　なってるはず...
	walls.shrink_to_fit();
	enemys.shrink_to_fit();

	// 壁とゴールをバッファに書き込む
	Goal goal(player_goal_point.X, player_goal_point.Y);
	renderer.set_screen_buffer(goal.get_curr_pos().X, goal.get_curr_pos().Y, goal.get_graphic(), goal.get_foreground_color(), goal.get_background_color());
	for (const auto& wall : walls)
		renderer.set_screen_buffer(wall.get_curr_pos().X, wall.get_curr_pos().Y, wall.get_graphic(), wall.get_foreground_color(), wall.get_background_color());

	FpsSustainer fps_sustainer;
	uint32_t frame_count = 0;
	Player player(player_start_point.X, player_start_point.Y);
	bool is_gameover = false;
	bool is_gameclear = false;
	while (1)
	{
		// 1 フレームの時間を確かめるため、現在時刻で初期化
		fps_sustainer.init_start_time();

		player.move();
		for (const auto& wall : walls)
		{
			// 壁と被っていたら前フレームの座標に戻す
			if (is_collided(player, wall))  // bool is_collided(Object a, Object b) -> Object.cpp に有り
				player.undo();
		}

		for (auto& enemy : enemys)
		{
			if (frame_count % 2)  // 敵の動く速さを調整
				enemy.move();
			for (auto& wall : walls)
			{
				if (is_collided(enemy, wall))
					enemy.undo(), enemy.set_reverse();
			}
			// ミス判定
			if (is_collided(player, enemy))
				is_gameover = true;
		}

		// バッファ書き込み
		for (const auto& enemy : enemys)
		{
			renderer.set_screen_buffer(enemy.get_prev_pos().X, enemy.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_screen_buffer(enemy.get_curr_pos().X, enemy.get_curr_pos().Y, enemy.get_graphic(), enemy.get_foreground_color(), enemy.get_background_color());
		}
		renderer.set_screen_buffer(player.get_prev_pos().X, player.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
		renderer.set_screen_buffer(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), player.get_foreground_color(), player.get_background_color());

		// 描画反映
		renderer.render();

		// ループ抜ける
		if (is_gameover)
			break;
		if (is_collided(player, goal))
		{
			is_gameclear = true;
			break;
		}

		frame_count = (frame_count + 1) % UINT16_MAX;  // 念の為、余裕を持って割っとく

		// 時間に余裕があれば待つ
		fps_sustainer.wait();
	}
}