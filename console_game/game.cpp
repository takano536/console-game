#include "game.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include "StlAllocator.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "Color.h"
#include "Player.h"
#include "Enemy.h"
#include "Wall.h"
#include "Goal.h"

static constexpr short BOTTOM_MARGIN = 5;  // �c�����̋󔒕�
static std::string map_filepath = "./map.txt";

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos);

void game_loop()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Enemy, StlAllocator<Enemy>> enemys;
	enemys.reserve(static_cast<std::vector<Enemy, StlAllocator<Enemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_pos = COORD{1, 1};
	COORD player_goal_pos = COORD{static_cast<short>(renderer.get_max_height() - BOTTOM_MARGIN - 1), static_cast<short>(renderer.get_max_width() - 1)};
	load_map(walls, enemys, player_start_pos, player_goal_pos);

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

int load_map(std::vector<Wall, StlAllocator<Wall>>& walls, std::vector<Enemy, StlAllocator<Enemy>>& enemys, COORD& player_start_pos, COORD& player_goal_pos)
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