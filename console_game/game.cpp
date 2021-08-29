#include "game.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include "StlAllocator.h"
#include "AsciiArt.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "Color.h"
#include "Choices.h"
#include "KeyPressDetector.h"
#include "Player.h"
#include "StraightEnemy.h"
#include "RandomEnemy.h"
#include "Wall.h"
#include "Goal.h"
#include "Cannon.h"

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

static constexpr LPCTSTR filepath = L"./settings.ini";
static const std::string map_filepath = "./map.txt";
static constexpr short BOTTOM_MARGIN = 5;  // 縦方向の空白幅

int load_map(
    std::vector<Wall, StlAllocator<Wall>>& walls,
    std::vector<StraightEnemy, StlAllocator<StraightEnemy>>& straight_enemys,
    std::vector<RandomEnemy, StlAllocator<RandomEnemy>>& random_enemys,
    std::vector<Cannon, StlAllocator<Cannon>>& cannons,
    COORD& player_start_pos,
    COORD& player_goal_pos);

void load_ini(int& level, int& speed);

std::string convert_time(uint32_t time);

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

static std::random_device rd;
static std::mt19937_64 mt64(rd());
uint64_t get_rand()
{
	return mt64();
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

int game_loop()
{
	Renderer renderer;
	renderer.init_window();

	std::vector<Wall, StlAllocator<Wall>> walls;
	walls.reserve(static_cast<std::vector<Wall, StlAllocator<Wall>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<StraightEnemy, StlAllocator<StraightEnemy>> straight_enemys;
	straight_enemys.reserve(static_cast<std::vector<StraightEnemy, StlAllocator<StraightEnemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<RandomEnemy, StlAllocator<RandomEnemy>> random_enemys;
	straight_enemys.reserve(static_cast<std::vector<RandomEnemy, StlAllocator<RandomEnemy>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	std::vector<Cannon, StlAllocator<Cannon>> cannons;
	cannons.reserve(static_cast<std::vector<Cannon, StlAllocator<Cannon>>::size_type>(renderer.get_max_width()) * renderer.get_max_height());
	COORD player_start_pos = COORD{1, 1};
	COORD player_goal_pos = COORD{static_cast<short>(renderer.get_max_height() - BOTTOM_MARGIN - 1), static_cast<short>(renderer.get_max_width() - 1)};
	if (load_map(walls, straight_enemys, random_enemys, cannons, player_start_pos, player_goal_pos) == -1)
		return -1;
	Goal goal(player_goal_pos.X, player_goal_pos.Y);
	std::vector<Bullet, StlAllocator<Bullet>> bullets;

	int level = 0, speed = 0;
	load_ini(level, speed);

	std::vector<Object, StlAllocator<Object>> obstacles;
	obstacles.reserve(walls.size() + cannons.size());
	obstacles.insert(obstacles.end(), walls.begin(), walls.end());
	obstacles.insert(obstacles.end(), cannons.begin(), cannons.end());

	FpsSustainer fps_sustainer;
	uint32_t frame_count = 0;
	uint32_t time = 0;

	Player player(player_start_pos.X, player_start_pos.Y);
	uint8_t player_move_interval = 0;  // 等間隔で動くために使用
	KeyPressDetector key_detector;

	bool is_gameover = false;
	bool is_gameclear = false;

	// スタートアニメーション
	std::array<AsciiArt, 4> start_art = {AsciiArt("3"), AsciiArt("2"), AsciiArt("1"), AsciiArt("start!!")};
	for (const auto& art : start_art)
	{
		renderer.init_screen_buffer();
		COORD rendering_start_pos = {static_cast<short>((renderer.get_max_width() - art.strings_len) * 0.5), static_cast<short>((renderer.get_max_height() - art.line_num) * 0.5)};
		for (int i = 0; i < art.line_num; i++)
			renderer.set_string(rendering_start_pos.X, rendering_start_pos.Y + i, art.ascii_art[i], Color::LIGHT_SKY_BLUE, Color::BLACK);
		renderer.render();
		Sleep(600);
	}
	Sleep(100);

	renderer.init_screen_buffer();
	std::string message = "ARROW KEY: MOVE   ESCAPE KEY: BACK TITLE";
	renderer.set_string((renderer.get_max_width() - 1) - static_cast<short>(message.size()) - 1, renderer.get_max_height() - 2, message, Color::WHITE, Color::BLACK);
	while (!is_gameover && !is_gameclear)
	{
		// 1 フレームの時間を確かめるため、現在時刻で初期化
		fps_sustainer.init_start_time();

		// escape が押された
		if (key_detector.get_curr_press().test(5) && !key_detector.get_prev_press().test(5))
			break;

		// プレイヤーの移動
		key_detector.update();
		if (key_detector.get_curr_press().count() > 0 && key_detector.get_curr_press().count() > key_detector.get_prev_press().count())
		{
			player.move(key_detector.get_last_pressed_key());
			player_move_interval = frame_count % (3 - speed);
		}
		else if (key_detector.get_curr_press().count() > 0 && frame_count % (3 - speed) == player_move_interval)
		{
			player.move(key_detector.get_last_pressed_key());
		}
		// 壁と被っていたら、前の座標に戻す
		for (auto& obstacle : obstacles)
			if (is_collided(player, obstacle))
				player.undo();

		auto iter = bullets.begin();
		while (iter != bullets.end())
		{
			bool is_deleted = false;
			for (const auto& obstacle : obstacles)
			{
				if (is_collided(*iter, obstacle))
				{
					iter = bullets.erase(iter);
					is_deleted = true;
					break;
				}
			}
			if (!is_deleted)
				iter++;
		}
		bullets.shrink_to_fit();
		for (auto& bullet : bullets)
			bullet.move();
		if (frame_count % (30 - level * 10) == 0)
		{
			for (auto& cannon : cannons)
			{
				// 弾の生成
				bullets.reserve(2);  // 2方向
				bullets.push_back(Bullet(cannon.get_curr_pos().X + 1, cannon.get_curr_pos().Y, 0));
				bullets.push_back(Bullet(cannon.get_curr_pos().X - 1, cannon.get_curr_pos().Y, 1));
			}
		}
		for (auto& bullet : bullets)
			if (is_collided(player, bullet))
				is_gameover = true;

		for (auto& enemy : straight_enemys)
		{
			if (frame_count % max((2 - level), 1) == 0)  // 敵の動く速さを調整
				enemy.move();
			// 壁と被っていたら、前の座標に戻す
			for (auto& obstacle : obstacles)
				if (is_collided(enemy, obstacle))
					enemy.undo(), enemy.set_reverse();
			// ミス判定
			if (is_collided(player, enemy))
				is_gameover = true;
		}
		for (auto& enemy : random_enemys)
		{
			if (frame_count % max((2 - level), 1) == 0)  // 敵の動く速さを調整
				enemy.move(get_rand());
			// 壁と被っていたら、前の座標に戻す
			for (auto& obstacle : obstacles)
				if (is_collided(enemy, obstacle))
					enemy.undo();
			// ミス判定
			if (is_collided(player, enemy))
				is_gameover = true;
		}

		// バッファ書き込み
		for (const auto& bullet : bullets)
		{
			renderer.set_char(bullet.get_prev_pos().X, bullet.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_char(bullet.get_curr_pos().X, bullet.get_curr_pos().Y, bullet.get_graphic(), bullet.get_foreground_color(), bullet.get_background_color());
		}
		for (const auto& enemy : straight_enemys)
		{
			renderer.set_char(enemy.get_prev_pos().X, enemy.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_char(enemy.get_curr_pos().X, enemy.get_curr_pos().Y, enemy.get_graphic(), enemy.get_foreground_color(), enemy.get_background_color());
		}
		for (const auto& enemy : random_enemys)
		{
			renderer.set_char(enemy.get_prev_pos().X, enemy.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
			renderer.set_char(enemy.get_curr_pos().X, enemy.get_curr_pos().Y, enemy.get_graphic(), enemy.get_foreground_color(), enemy.get_background_color());
		}
		renderer.set_char(goal.get_curr_pos().X, goal.get_curr_pos().Y, goal.get_graphic(), goal.get_foreground_color(), goal.get_background_color());
		renderer.set_char(player.get_prev_pos().X, player.get_prev_pos().Y, ' ', Color::WHITE, Color::BLACK);
		renderer.set_char(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), player.get_foreground_color(), player.get_background_color());
		for (const auto& wall : walls)
			renderer.set_char(wall.get_curr_pos().X, wall.get_curr_pos().Y, wall.get_graphic(), wall.get_foreground_color(), wall.get_background_color());
		for (const auto& cannon : cannons)
			renderer.set_char(cannon.get_curr_pos().X, cannon.get_curr_pos().Y, cannon.get_graphic(), cannon.get_foreground_color(), cannon.get_background_color());
		std::string time_str = "TIME " + convert_time(time);
		renderer.set_string(static_cast<short>((renderer.get_max_width() - time_str.size()) * 0.5), static_cast<short>(renderer.get_max_height() - std::ceil(static_cast<double>(BOTTOM_MARGIN) / 2)), time_str, Color::WHITE, Color::BLACK);

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

		frame_count = (frame_count + 1) % fps_sustainer.get_fps();
		if (frame_count == 0)
			time = (time + 1) % UINT32_MAX;

		// 時間に余裕があれば待つ
		fps_sustainer.wait();
	}

	// escape
	if (!is_gameover && !is_gameclear)
	{
		Sleep(500);
		return 0;
	}

	// プレイヤー以外の色をモノクロにする
	if (is_gameover)
		renderer.change_monochrome();

	// 2 秒のアニメーション
	uint16_t animation_seconds = 1;
	Color curr_fore_color = player.get_foreground_color();
	Color curr_back_color = player.get_background_color();
	const uint16_t half_fps = static_cast<uint16_t>(fps_sustainer.get_fps() * 0.5);
	for (frame_count = 0; frame_count <= static_cast<uint32_t>(fps_sustainer.get_fps() * animation_seconds); frame_count++)
	{
		fps_sustainer.init_start_time();

		// 0.5 秒ごとにプレイヤーを点滅させる
		if (frame_count % half_fps == 0)
		{
			std::swap(curr_fore_color, curr_back_color);
			renderer.set_char(player.get_curr_pos().X, player.get_curr_pos().Y, player.get_graphic(), curr_fore_color, curr_back_color);
		}
		renderer.render();

		fps_sustainer.wait();
	}
	Sleep(500);

	if (is_gameover)
		return 1;
	else if (is_gameclear)
		return 2;
	return 0;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

int game_end_loop(std::string display_str)
{
	Renderer renderer;
	renderer.init_window();

	AsciiArt clear_art(display_str);
	COORD rendering_start_pos = {static_cast<short>((renderer.get_max_width() - clear_art.strings_len) * 0.5), static_cast<short>((renderer.get_max_height() - clear_art.line_num) * 0.5)};
	for (int i = 0; i < clear_art.line_num; i++)
		renderer.set_string(rendering_start_pos.X, rendering_start_pos.Y + i, clear_art.ascii_art[i], Color::WHITE, Color::BLACK);
	renderer.render();
	Sleep(500);

	Choices choices({"      RETRY      ", "BACK TO THE TITLE"});

	// 選択肢の表示と取得
	return choices.choices_loop(5, renderer);
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

void load_ini(int& level, int& speed)
{
	const int str_size = 128;
	TCHAR level_str[str_size], speed_str[str_size];
	GetPrivateProfileString(L"game", L"level", L"", level_str, str_size, L"./settings.ini");
	GetPrivateProfileString(L"game", L"speed", L"", speed_str, str_size, L"./settings.ini");
	level = _wtoi(level_str);
	speed = _wtoi(speed_str);
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

int load_map(
    std::vector<Wall, StlAllocator<Wall>>& walls,
    std::vector<StraightEnemy, StlAllocator<StraightEnemy>>& straight_enemys,
    std::vector<RandomEnemy, StlAllocator<RandomEnemy>>& random_enemys,
    std::vector<Cannon, StlAllocator<Cannon>>& cannons,
    COORD& player_start_pos,
    COORD& player_goal_pos)
{
	Renderer renderer;

	std::set<std::pair<int, int>> walls_pos_set;  // 壁の座標がかぶる可能性があるので、set で管理

	// 一番外の壁がないと大変なことになるので、最初から入れとく
	for (int i = 0; i < renderer.get_max_height() - BOTTOM_MARGIN; i++)  // 左の壁
		walls_pos_set.insert(std::make_pair(0, i));
	for (int i = 0; i < renderer.get_max_height() - BOTTOM_MARGIN; i++)  // 右の壁
		walls_pos_set.insert(std::make_pair(renderer.get_max_width() - 1, i));
	for (int i = 0; i < renderer.get_max_width(); i++)  // 上の壁
		walls_pos_set.insert(std::make_pair(i, 0));
	for (int i = 0; i < renderer.get_max_width(); i++)  // 下の壁
		walls_pos_set.insert(std::make_pair(i, renderer.get_max_height() - 1 - BOTTOM_MARGIN));

	// map.txt からマップ情報を読み込む
	std::ifstream ifs(map_filepath);
	if (ifs.fail())
	{
		std::string message = "Failed to open \"map.txt\"...";
		renderer.set_string(static_cast<short>(renderer.get_max_height() * 0.5), static_cast<short>((renderer.get_max_width() - message.size()) * 0.5), message, Color::WHITE, Color::BLACK);
		Sleep(10000);
		return -1;
	}
	std::string str;
	short i = 0, j = 0;
	while (std::getline(ifs, str))
	{
		for (auto c : str)
		{
			if (i > renderer.get_max_height() - BOTTOM_MARGIN || j > renderer.get_max_width())
				continue;
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
					straight_enemys.push_back(StraightEnemy(j, i, true));
					break;
				case 'e':
					straight_enemys.push_back(StraightEnemy(j, i, false));
					break;
				case 'R':
					random_enemys.push_back(RandomEnemy(j, i));
					break;
				case '-':
					cannons.push_back(Cannon(j, i));
					break;
				default:
					break;
			}
			j++;
		}
		j = 0;
		i++;
	}

	// walls の set を引数の vector にコピー
	for (const auto& pos : walls_pos_set)
		walls.push_back(Wall(pos.first, pos.second));

	// vector のメモリ節約　なってるはず...
	walls.shrink_to_fit();
	straight_enemys.shrink_to_fit();
	random_enemys.shrink_to_fit();
	cannons.shrink_to_fit();

	return 0;
}

std::string convert_time(uint32_t time)
{
	std::ostringstream oss_hours, oss_minutes, oss_seconds;
	uint32_t hours = time / 3600;
	oss_hours << std::setfill('0') << std::setw(2) << hours;
	uint32_t minutes = (time % 3600) / 60;
	oss_minutes << std::setfill('0') << std::setw(2) << minutes;
	uint32_t seconds = time % 3600 % 60;
	oss_seconds << std::setfill('0') << std::setw(2) << seconds;

	return oss_hours.str() + ':' + oss_minutes.str() + ':' + oss_seconds.str();
}