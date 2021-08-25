#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "StlAllocator.h"
#include "FpsSustainer.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Goal.h"

constexpr short MAX_WIDTH = 120;                                   // 最大横幅
constexpr short MAX_HEIGHT = 30;                                   // 最大縦幅
constexpr short HORIZONTAL_MARGIN = 20;                            // 横方向の空白幅
constexpr short VERTICAL_MARGIN = 5;                               // 縦方向の空白幅
constexpr COORD BUFFER_SIZE = {MAX_WIDTH, MAX_HEIGHT};             // バッファーサイズ
constexpr COORD START_COORD = {0, 0};                              // 描画開始位置
constexpr CONSOLE_CURSOR_INFO INVISIBLE_CURSOR_INFO = {1, FALSE};  // 不可視カーソルの引数
constexpr char map_filepath[] = "./map.txt";                       // マップデータのファイルパス

int main()
{
	const SMALL_RECT WINDOW_SIZE = {0, 0, MAX_WIDTH - 1, MAX_HEIGHT - 1};  // 画面サイズ
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);                       // スクリーンバッファのハンドル

	// cmd.exe の設定の指定
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &INVISIBLE_CURSOR_INFO);  // カーソルを不可視化
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), BUFFER_SIZE);       // バッファサイズの指定
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &WINDOW_SIZE);      // ウインドウサイズの指定

	CHAR_INFO write_chars[MAX_HEIGHT][MAX_WIDTH] = {};  // スクリーンバッファに書き込む文字

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(MAX_WIDTH * MAX_HEIGHT);
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(MAX_WIDTH * MAX_HEIGHT);
	COORD player_start_point = COORD{1, 1};
	COORD player_goal_point = COORD{MAX_HEIGHT - VERTICAL_MARGIN - 1, MAX_WIDTH - HORIZONTAL_MARGIN - 1};

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
			if (i > MAX_HEIGHT - VERTICAL_MARGIN || j > MAX_WIDTH - HORIZONTAL_MARGIN)
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
	write_chars[goal.get_curr_pos().Y][goal.get_curr_pos().X].Char.AsciiChar = goal.get_graphic();
	write_chars[goal.get_curr_pos().Y][goal.get_curr_pos().X].Attributes = goal.get_foreground_color() + goal.get_background_color() * 16;
	for (const auto& wall : walls)
	{
		write_chars[wall.get_curr_pos().Y][wall.get_curr_pos().X].Char.AsciiChar = wall.get_graphic();
		write_chars[wall.get_curr_pos().Y][wall.get_curr_pos().X].Attributes = wall.get_foreground_color() + wall.get_background_color() * 16;
	}

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
			write_chars[enemy.get_prev_pos().Y][enemy.get_prev_pos().X].Char.AsciiChar = ' ';
			write_chars[enemy.get_prev_pos().Y][enemy.get_prev_pos().X].Attributes = enemy.get_foreground_color() + enemy.get_background_color() * 16;
			write_chars[enemy.get_curr_pos().Y][enemy.get_curr_pos().X].Char.AsciiChar = enemy.get_graphic();
			write_chars[enemy.get_curr_pos().Y][enemy.get_curr_pos().X].Attributes = enemy.get_foreground_color() + enemy.get_background_color() * 16;
		}
		write_chars[player.get_prev_pos().Y][player.get_prev_pos().X].Char.AsciiChar = ' ';
		write_chars[player.get_prev_pos().Y][player.get_prev_pos().X].Attributes = player.get_foreground_color() + player.get_background_color() * 16;
		write_chars[player.get_curr_pos().Y][player.get_curr_pos().X].Char.AsciiChar = player.get_graphic();
		write_chars[player.get_curr_pos().Y][player.get_curr_pos().X].Attributes = player.get_foreground_color() + player.get_background_color() * 16;

		// 描画反映
		WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), reinterpret_cast<CHAR_INFO*>(write_chars), BUFFER_SIZE, START_COORD, const_cast<SMALL_RECT*>(&WINDOW_SIZE));

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