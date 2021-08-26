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

constexpr short HORIZONTAL_MARGIN = 20;  // �������̋󔒕�
constexpr short VERTICAL_MARGIN = 5;     // �c�����̋󔒕�
std::string map_filepath = "./map.txt";

int title_loop()
{
	Renderer renderer;
	renderer.init_window();

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
	short tmp = 0;
	for (const auto& str : title_name)
		tmp = max(static_cast<short>(str.size()), tmp);
	const short line_num = static_cast<short>(title_name.size());
	const short max_title_string_len = tmp;

	// �^�C�g���A�j���[�V����
	const short VERTICAL_MARGIN = 5;  // �c�����̋󔒕�
	COORD title_rendering_start_point = {static_cast<short>((renderer.get_max_width() - max_title_string_len) * 0.5 - 1), 0};
	for (int i = 0; i < line_num + VERTICAL_MARGIN; i++)
	{
		if (i - line_num == 0)
			continue;
		renderer.init_screen_buffer();
		title_rendering_start_point.Y = max(i - line_num, 0);
		for (int j = max(line_num - i - 1, 0), line_cnt = 0; j < line_num; j++, line_cnt++)
			renderer.set_string(title_rendering_start_point.X, title_rendering_start_point.Y + line_cnt, title_name[j], Color::LIGHT_SKY_BLUE, Color::BLACK);
		renderer.render();
		Sleep(100);
	}
	renderer.init_screen_buffer();
	for (int i = 0; i < line_num; i++)
		renderer.set_string(title_rendering_start_point.X, (VERTICAL_MARGIN - 1) + i, title_name[i], Color::LIGHT_SKY_BLUE, Color::BLACK);
	renderer.render();
	Sleep(100);

	std::vector<std::string, StlAllocator<std::string>> menu = {
	    "GAME START",
	    "  OPTION  ",
	    "   QUIT   ",
	};
	tmp = 0;
	for (const auto& str : menu)
		tmp = max(static_cast<short>(str.size()), tmp);
	const short menu_num = static_cast<short>(menu.size());
	const short max_menu_string_len = tmp;

	COORD menu_rendering_start_point = {static_cast<short>((renderer.get_max_width() - max_menu_string_len) * 0.5 - 1), renderer.get_max_height() - menu_num - VERTICAL_MARGIN};
	for (int i = 0; i < menu_num; i++)
		renderer.set_string(menu_rendering_start_point.X, menu_rendering_start_point.Y + i, menu[i], Color::WHITE, Color::BLACK);

	std::string operation_method_description = "ARROW KEY: MOVE   ENTER KEY: SELECT";
	renderer.set_string((renderer.get_max_width() - 1) - static_cast<short>(operation_method_description.size()) - 1, renderer.get_max_height() - 2, operation_method_description, Color::WHITE, Color::BLACK);

	short curr_cursor = 0;
	std::array<COORD, 2> cursor_rendering_point = {
	    COORD{static_cast<short>(menu_rendering_start_point.X - 3), menu_rendering_start_point.Y},
	    COORD{static_cast<short>(menu_rendering_start_point.X + max_menu_string_len + 1), menu_rendering_start_point.Y},
	};
	FpsSustainer fps_sustainer;
	bool is_selected = false;
	while (!is_selected)
	{
		fps_sustainer.init_start_time();

		if (GetAsyncKeyState(VK_DOWN) & 1)
			curr_cursor = (curr_cursor + 1) % menu_num;
		else if (GetAsyncKeyState(VK_UP) & 1)
			curr_cursor = curr_cursor - 1 < 0 ? menu_num - 1 : curr_cursor - 1;
		else if (GetAsyncKeyState(VK_RETURN) & 1)
			is_selected = true;

		for (int i = 0; i < menu_num; i++)
		{
			renderer.set_string(cursor_rendering_point[0].X, cursor_rendering_point[0].Y + i, "  ", Color::WHITE, Color::BLACK);
			renderer.set_string(cursor_rendering_point[1].X, cursor_rendering_point[1].Y + i, "  ", Color::WHITE, Color::BLACK);
		}
		renderer.set_string(cursor_rendering_point[0].X, cursor_rendering_point[0].Y + curr_cursor, "->", Color::WHITE, Color::BLACK);
		renderer.set_string(cursor_rendering_point[1].X, cursor_rendering_point[1].Y + curr_cursor, "<-", Color::WHITE, Color::BLACK);
		renderer.render();

		// ���Ԃɗ]�T������Α҂�
		fps_sustainer.wait();
	}

	return curr_cursor;
}

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_point, COORD& player_goal_point)
{
	Renderer renderer;

	// map.txt ����}�b�v����ǂݍ���
	std::ifstream ifs(map_filepath);
	if (ifs.fail())
	{
		std::cerr << "Failed to open \"map.txt\"..." << std::endl;
		return -1;
	}
	std::string str;
	short i = 0, j = 0;
	bool is_failed = false;  // �t�@�C���ǂݍ��݂����s�������ǂ���
	while (std::getline(ifs, str))
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
		return -1;
	}

	return 0;
}

void option_loop()
{
	Renderer renderer;
	renderer.init_window();
}

void game_loop()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(static_cast<std::vector<Enemy, StlAllocator<Enemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_point = COORD{1, 1};
	COORD player_goal_point = COORD{static_cast<short>(renderer.get_max_height() - VERTICAL_MARGIN - 1), static_cast<short>(renderer.get_max_width() - HORIZONTAL_MARGIN - 1)};
	load_map(walls, enemys, player_start_point, player_goal_point);

	// vector �̃������ߖ�@�Ȃ��Ă�͂�...
	walls.shrink_to_fit();
	enemys.shrink_to_fit();

	// �ǂƃS�[�����o�b�t�@�ɏ�������
	Goal goal(player_goal_point.X, player_goal_point.Y);
	renderer.set_char(goal.get_curr_pos().X, goal.get_curr_pos().Y, goal.get_graphic(), goal.get_foreground_color(), goal.get_background_color());
	for (const auto& wall : walls)
		renderer.set_char(wall.get_curr_pos().X, wall.get_curr_pos().Y, wall.get_graphic(), wall.get_foreground_color(), wall.get_background_color());

	FpsSustainer fps_sustainer;
	uint32_t frame_count = 0;
	Player player(player_start_point.X, player_start_point.Y);
	bool is_gameover = false;
	bool is_gameclear = false;
	while (!is_gameover && !is_gameclear)
	{
		// 1 �t���[���̎��Ԃ��m���߂邽�߁A���ݎ����ŏ�����
		fps_sustainer.init_start_time();

		player.move();
		for (const auto& wall : walls)
		{
			// �ǂƔ���Ă�����O�t���[���̍��W�ɖ߂�
			if (is_collided(player, wall))  // bool is_collided(Object a, Object b) -> Object.cpp �ɗL��
				player.undo();
		}

		for (auto& enemy : enemys)
		{
			if (frame_count % 2)  // �G�̓��������𒲐�
				enemy.move();
			for (auto& wall : walls)
			{
				if (is_collided(enemy, wall))
					enemy.undo(), enemy.set_reverse();
			}
			// �~�X����
			if (is_collided(player, enemy))
				is_gameover = true;
		}

		// �o�b�t�@��������
		for (const auto& enemy : enemys)
		{
			renderer.set_char(enemy.get_prev_pos().X, enemy.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_char(enemy.get_curr_pos().X, enemy.get_curr_pos().Y, enemy.get_graphic(), enemy.get_foreground_color(), enemy.get_background_color());
		}
		renderer.set_char(player.get_prev_pos().X, player.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
		renderer.set_char(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), player.get_foreground_color(), player.get_background_color());

		// �`�攽�f
		renderer.render();

		// ���[�v������
		if (is_gameover)
			break;
		if (is_collided(player, goal))
		{
			is_gameclear = true;
			break;
		}

		frame_count = (frame_count + 1) % UINT16_MAX;  // �O�ׁ̈A�]�T�������Ċ����Ƃ�

		// ���Ԃɗ]�T������Α҂�
		fps_sustainer.wait();
	}
}