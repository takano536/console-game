#include "game.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <windows.h>
#include "StlAllocator.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "Color.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Goal.h"

#include "get_selected_choices.h"

static constexpr short BOTTOM_MARGIN = 5;  // �c�����̋󔒕�
static std::string map_filepath = "./map.txt";

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos);

int game_loop()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(static_cast<std::vector<Enemy, StlAllocator<Enemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_pos = COORD{1, 1};
	COORD player_goal_pos = COORD{static_cast<short>(renderer.get_max_height() - BOTTOM_MARGIN - 1), static_cast<short>(renderer.get_max_width() - 1)};
	if (load_map(walls, enemys, player_start_pos, player_goal_pos) == -1)
		return -1;

	// vector �̃������ߖ�@�Ȃ��Ă�͂�...
	walls.shrink_to_fit();
	enemys.shrink_to_fit();

	// �ǂƃS�[�����o�b�t�@�ɏ�������
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
		// 1 �t���[���̎��Ԃ��m���߂邽�߁A���ݎ����ŏ�����
		fps_sustainer.init_start_time();

		player.move();
		// �ǂƔ���Ă�����A�O�̍��W�ɖ߂�
		if (renderer.get_char(player.get_curr_pos().X, player.get_curr_pos().Y) == '#')
			player.undo();

		for (auto& enemy : enemys)
		{
			if (frame_count % 2)  // �G�̓��������𒲐�
				enemy.move();
			// �ǂƔ���Ă�����A�O�̍��W�ɖ߂�
			if (renderer.get_char(enemy.get_curr_pos().X, enemy.get_curr_pos().Y) == '#')
				enemy.undo(), enemy.set_reverse();
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

	// �Q�[���I�[�o�[
	if (is_gameover)
	{
		// �v���C���[�ȊO�̐F�����m�N���ɂ���
		renderer.change_monochrome();

		// 2 �b�̃A�j���[�V����
		uint16_t animation_seconds = 1;
		Color curr_fore_color = player.get_foreground_color();
		Color curr_back_color = player.get_background_color();
		const uint16_t half_fps = fps_sustainer.get_fps() * 0.5;
		for (frame_count = 0; frame_count <= fps_sustainer.get_fps() * animation_seconds; frame_count++)
		{
			fps_sustainer.init_start_time();

			// 0.5 �b���ƂɃv���C���[��_�ł�����
			if (frame_count % half_fps == 0)
			{
				std::swap(curr_fore_color, curr_back_color);
				renderer.set_char(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), curr_fore_color, curr_back_color);
			}
			renderer.render();

			fps_sustainer.wait();
		}
		Sleep(500);
		return 1;
	}

	// �Q�[���N���A
	Sleep(500);
	return 2;
}

int gameclear_loop()
{
	Renderer renderer;
	renderer.init_window();

	// �Q�[���N���A�����f�[�^
	std::vector<std::string, StlAllocator<std::string>> gameover_str = {
	    "  .oooooo.   ooooo        oooooooooooo       .o.       ooooooooo.   888 888",
	    " d8P'  `Y8b  `888'        `888'     `8      .888.      `888   `Y88. 888 888",
	    "888           888          888             .8\"888.      888   .d88' 888 888",
	    "888           888          888oooo8       .8' `888.     888ooo88P'  888 888",
	    "888           888          888    \"      .88ooo8888.    888`88b.           ",
	    "`88b    ooo   888       o  888       o  .8'     `888.   888  `88b.  d8b d8b",
	    " `Y8bood8P'  o888ooooood8 o888ooooood8 o88o     o8888o o888o  o888o Y8P Y8P",
	};
	const short line_num = static_cast<short>(gameover_str.size());
	COORD rendering_start_pos = {(renderer.get_max_width() - gameover_str[0].size()) * 0.5, (renderer.get_max_height() - line_num) * 0.5};
	for (int i = 0; i < line_num; i++)
		renderer.set_string(rendering_start_pos.X, rendering_start_pos.Y + i, gameover_str[i], Color::WHITE, Color::BLACK);
	renderer.render();
	Sleep(500);

	std::vector<std::string, StlAllocator<std::string>> choices = {
	    "      RETRY      ",
	    "BACK TO THE TITLE",
	};
	short choices_strings_max_len = get_strings_max_len(choices);
	short rendering_start_pos_x = static_cast<short>((renderer.get_max_width() - choices_strings_max_len) * 0.5 - 1);
	short rendering_start_pos_y = static_cast<short>(renderer.get_max_height() - choices.size() - 5);

	// �I�����̕\���Ǝ擾
	return get_selected_choices(renderer, choices, rendering_start_pos_x, rendering_start_pos_y);
}

int gameover_loop()
{
	Renderer renderer;
	renderer.init_window();

	// �Q�[���I�[�o�[�����f�[�^
	std::vector<std::string, StlAllocator<std::string>> gameover_str = {
	    "  .oooooo.          .o.       ooo        ooooo oooooooooooo      .oooooo.   oooooo     oooo oooooooooooo ooooooooo.  ",
	    " d8P'  `Y8b        .888.      `88.       .888' `888'     `8     d8P'  `Y8b   `888.     .8'  `888'     `8 `888   `Y88.",
	    "888               .8\"888.      888b     d'888   888            888      888   `888.   .8'    888          888   .d88'",
	    "888              .8' `888.     8 Y88. .P  888   888oooo8       888      888    `888. .8'     888oooo8     888ooo88P' ",
	    "888     ooooo   .88ooo8888.    8  `888'   888   888    \"       888      888     `888.8'      888    \"     888`88b.	  ",
	    "`88.    .88'   .8'     `888.   8    Y     888   888       o    `88b    d88'      `888'       888       o  888  `88b. ",
	    " `Y8bood8P'   o88o     o8888o o8o        o888o o888ooooood8     `Y8bood8P'        `8'       o888ooooood8 o888o  o888o",
	};
	const short line_num = static_cast<short>(gameover_str.size());
	COORD rendering_start_pos = {(renderer.get_max_width() - gameover_str[0].size()) * 0.5, (renderer.get_max_height() - line_num) * 0.5};
	for (int i = 0; i < line_num; i++)
		renderer.set_string(rendering_start_pos.X, rendering_start_pos.Y + i, gameover_str[i], Color::WHITE, Color::BLACK);
	renderer.render();
	Sleep(500);

	std::vector<std::string, StlAllocator<std::string>> choices = {
	    "      RETRY      ",
	    "BACK TO THE TITLE",
	};
	short choices_strings_max_len = get_strings_max_len(choices);
	short rendering_start_pos_x = static_cast<short>((renderer.get_max_width() - choices_strings_max_len) * 0.5 - 1);
	short rendering_start_pos_y = static_cast<short>(renderer.get_max_height() - choices.size() - 5);

	// �I�����̕\���Ǝ擾
	return get_selected_choices(renderer, choices, rendering_start_pos_x, rendering_start_pos_y);
}

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos)
{
	Renderer renderer;

	std::set<std::pair<int, int>> walls_pos_set;  // �ǂ̍��W�����Ԃ�\��������̂ŁAset �ŊǗ�

	// ��ԊO�̕ǂ��Ȃ��Ƒ�ςȂ��ƂɂȂ�̂ŁA�ŏ��������Ƃ�
	for (int i = 0; i < renderer.get_max_height() - BOTTOM_MARGIN; i++)  // ���̕�
		walls_pos_set.insert(std::make_pair(0, i));
	for (int i = 0; i < renderer.get_max_height() - BOTTOM_MARGIN; i++)  // �E�̕�
		walls_pos_set.insert(std::make_pair(renderer.get_max_width() - 1, i));
	for (int i = 0; i < renderer.get_max_width(); i++)  // ��̕�
		walls_pos_set.insert(std::make_pair(i, 0));
	for (int i = 0; i < renderer.get_max_width(); i++)  // ���̕�
		walls_pos_set.insert(std::make_pair(i, renderer.get_max_height() - 1 - BOTTOM_MARGIN));

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
			if (i > renderer.get_max_height() - BOTTOM_MARGIN || j > renderer.get_max_width())
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
					walls_pos_set.insert(std::make_pair(j, i));
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

	// walls �� set �������� vector �ɃR�s�[
	for (const auto& pos : walls_pos_set)
		walls.push_back(Wall(pos.first, pos.second));

	return 0;
}