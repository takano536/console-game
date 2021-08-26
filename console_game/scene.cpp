#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>
#include <conio.h>
#include "scene.h"
#include "StlAllocator.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "Color.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Goal.h"

constexpr short HORIZONTAL_MARGIN = 20;  // 横方向の空白幅
constexpr short VERTICAL_MARGIN = 5;     // 縦方向の空白幅
std::string map_filepath = "./map.txt";

int get_selected_choices(Renderer& renderer, const std::vector<std::string, StlAllocator<std::string>>& choices, short x, short y);
int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos);
short get_string_max_len(const std::vector<std::string, StlAllocator<std::string>>& strings);

int title_loop()
{
	Renderer renderer;
	renderer.init_window();

	// タイトルデータ
	std::vector<std::string, StlAllocator<std::string>> title_name = {
	    "Y88b   d88P  .d88888b.  888    d8P  8888888888 8888888b.   .d88888b.    888",
	    " Y88b d88P  d88P\" \"Y88b 888   d8P   888        888   Y88b d88P\" \"Y88b   888",
	    "  Y88o88P   888     888 888  d8P    888        888    888 888     888   888",
	    "   Y888P    888     888 888d88K     8888888    888   d88P 888     888   888",
	    "    888     888     888 8888888b    888        8888888P\"  888     888   888",
	    "    888     888     888 888  Y88b   888        888 T88b   888     888      ",
	    "    888     Y88b. .d88P 888   Y88b  888        888  T88b  Y88b. .d88P   d8b",
	    "    888      \"Y88888P\"  888    Y88b 8888888888 888   T88b  \"Y88888P\"    Y8P",
	};
	const short line_num = static_cast<short>(title_name.size());
	const short title_strings_max_len = get_string_max_len(title_name);

	// タイトルアニメーション
	COORD title_rendering_start_pos = {static_cast<short>((renderer.get_max_width() - title_strings_max_len) * 0.5 - 1), 0};
	for (int i = 0; i < line_num + VERTICAL_MARGIN; i++)
	{
		if (i - line_num == 0)
			continue;
		renderer.init_screen_buffer();
		title_rendering_start_pos.Y = max(i - line_num, 0);
		for (int j = max(line_num - i - 1, 0), line_cnt = 0; j < line_num; j++, line_cnt++)
			renderer.set_string(title_rendering_start_pos.X, title_rendering_start_pos.Y + line_cnt, title_name[j], Color::LIGHT_SKY_BLUE, Color::BLACK);
		renderer.render();
		Sleep(100);
	}
	renderer.init_screen_buffer();
	for (int i = 0; i < line_num; i++)
		renderer.set_string(title_rendering_start_pos.X, (VERTICAL_MARGIN - 1) + i, title_name[i], Color::LIGHT_SKY_BLUE, Color::BLACK);
	renderer.render();
	Sleep(100);

	// 選択肢データ
	std::vector<std::string, StlAllocator<std::string>> choices = {
	    "GAME START",
	    "  OPTION  ",
	    "   QUIT   ",
	};
	short choices_strings_max_len = get_string_max_len(choices);
	short rendering_start_pos_x = static_cast<short>((renderer.get_max_width() - choices_strings_max_len) * 0.5 - 1);
	short rendering_start_pos_y = renderer.get_max_height() - choices.size() - VERTICAL_MARGIN;

	// 選択肢の表示と取得
	return get_selected_choices(renderer, choices, rendering_start_pos_x, rendering_start_pos_y);
}

void option_loop()
{
	Renderer renderer;
	renderer.init_window();

	// 選択肢データ
	std::vector<std::string, StlAllocator<std::string>> choices = {
	    "MAP DATA FILE PATH",
	    " PLAYER MOVEMENTS ",
	    "    DIFFICULTY    ",
	    "       BACK       ",
	};
	short choices_strings_max_len = get_string_max_len(choices);
	short rendering_start_pos_x = (renderer.get_max_width() - choices_strings_max_len) * 0.5 - 1;
	short rendering_start_pos_y = (renderer.get_max_height() - choices.size()) * 0.5;

	bool is_continue = true;
	while (is_continue)
	{
		short selected_choices = get_selected_choices(renderer, choices, rendering_start_pos_x, rendering_start_pos_y);
		switch (selected_choices)
		{
			case 0:
				break;

			case 1:
				break;

			case 2:
				break;

			case 3:
				is_continue = false;
				break;

			default:
				break;
		}
	}
}

void game_loop()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(static_cast<std::vector<Enemy, StlAllocator<Enemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_pos = COORD{1, 1};
	COORD player_goal_pos = COORD{static_cast<short>(renderer.get_max_height() - VERTICAL_MARGIN - 1), static_cast<short>(renderer.get_max_width() - HORIZONTAL_MARGIN - 1)};
	load_map(walls, enemys, player_start_pos, player_goal_pos);

	// vector のメモリ節約　なってるはず...
	walls.shrink_to_fit();
	enemys.shrink_to_fit();

	// 壁とゴールをバッファに書き込む
	Goal goal(player_goal_pos.X, player_goal_pos.Y);
	renderer.set_char(goal.get_curr_pos().X, goal.get_curr_pos().Y, goal.get_graphic(), goal.get_foreground_color(), goal.get_background_color());
	for (const auto& wall : walls)
		renderer.set_char(wall.get_curr_pos().X, wall.get_curr_pos().Y, wall.get_graphic(), wall.get_foreground_color(), wall.get_background_color());

	FpsSustainer fps_sustainer;
	uint32_t frame_count = 0;
	Player player(player_start_pos.X, player_start_pos.Y);
	bool is_gameover = false;
	bool is_gameclear = false;
	while (!is_gameover && !is_gameclear)
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
			renderer.set_char(enemy.get_prev_pos().X, enemy.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_char(enemy.get_curr_pos().X, enemy.get_curr_pos().Y, enemy.get_graphic(), enemy.get_foreground_color(), enemy.get_background_color());
		}
		renderer.set_char(player.get_prev_pos().X, player.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
		renderer.set_char(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), player.get_foreground_color(), player.get_background_color());

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

short get_string_max_len(const std::vector<std::string, StlAllocator<std::string>>& strings)
{
	short ret = 0;
	for (const auto& str : strings)
		ret = max(static_cast<short>(str.size()), ret);
	return ret;
}

int get_selected_choices(Renderer& renderer, const std::vector<std::string, StlAllocator<std::string>>& choices, short x, short y)
{
	const short choices_num = static_cast<short>(choices.size());
	const short max_choices_string_len = get_string_max_len(choices);

	COORD choices_rendering_start_pos = {x, y};
	for (int i = 0; i < choices_num; i++)
		renderer.set_string(choices_rendering_start_pos.X, choices_rendering_start_pos.Y + i, choices[i], Color::WHITE, Color::BLACK);

	std::string operation_method_description = "ARROW KEY: MOVE   ENTER KEY: SELECT";
	renderer.set_string((renderer.get_max_width() - 1) - static_cast<short>(operation_method_description.size()) - 1, renderer.get_max_height() - 2, operation_method_description, Color::WHITE, Color::BLACK);

	short curr_cursor = 0;
	std::array<COORD, 2> cursor_rendering_pos = {
	    COORD{static_cast<short>(choices_rendering_start_pos.X - 3), choices_rendering_start_pos.Y},
	    COORD{static_cast<short>(choices_rendering_start_pos.X + max_choices_string_len + 1), choices_rendering_start_pos.Y},
	};
	FpsSustainer fps_sustainer;
	bool is_selected = false;
	while (!is_selected)
	{
		fps_sustainer.init_start_time();

		if (GetAsyncKeyState(VK_DOWN) & 1)
			curr_cursor = (curr_cursor + 1) % choices_num;
		else if (GetAsyncKeyState(VK_UP) & 1)
			curr_cursor = curr_cursor - 1 < 0 ? choices_num - 1 : curr_cursor - 1;
		else if (GetAsyncKeyState(VK_RETURN) & 1)
			is_selected = true;

		for (int i = 0; i < choices_num; i++)
		{
			renderer.set_string(cursor_rendering_pos[0].X, cursor_rendering_pos[0].Y + i, "  ", Color::WHITE, Color::BLACK);
			renderer.set_string(cursor_rendering_pos[1].X, cursor_rendering_pos[1].Y + i, "  ", Color::WHITE, Color::BLACK);
		}
		renderer.set_string(cursor_rendering_pos[0].X, cursor_rendering_pos[0].Y + curr_cursor, "->", Color::WHITE, Color::BLACK);
		renderer.set_string(cursor_rendering_pos[1].X, cursor_rendering_pos[1].Y + curr_cursor, "<-", Color::WHITE, Color::BLACK);
		renderer.render();

		// 時間に余裕があれば待つ
		fps_sustainer.wait();
	}

	return curr_cursor;
}

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos)
{
	Renderer renderer;

	// map.txt からマップ情報を読み込む
	std::ifstream ifs(map_filepath);
	if (ifs.fail())
	{
		std::cerr << "Failed to open \"map.txt\"..." << std::endl;
		return -1;
	}
	std::string str;
	short i = 0, j = 0;
	bool is_failed = false;  // ファイル読み込みが失敗したかどうか
	while (std::getline(ifs, str))
	{
		for (auto c : str)
		{
			if (i > renderer.get_max_height() - VERTICAL_MARGIN || j > renderer.get_max_width() - HORIZONTAL_MARGIN)
				is_failed = true;
			switch (c)
			{
				case 'S':
					player_start_pos = COORD{j, i};
					break;
				case 'G':
					player_goal_pos = COORD{j, i};
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
		return -1;
	}

	return 0;
}