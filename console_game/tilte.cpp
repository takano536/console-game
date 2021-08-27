#include "title.h"

#include <string>
#include <vector>
#include <array>
#include <windows.h>
#include "StlAllocator.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "Color.h"

static constexpr short TITLE_VERTICAL_MARGIN = 5;  // 縦方向の空白幅

int get_selected_choices(Renderer& renderer, const std::vector<std::string, StlAllocator<std::string>>& choices, short x, short y);
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
	for (int i = 0; i < line_num + TITLE_VERTICAL_MARGIN; i++)
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
		renderer.set_string(title_rendering_start_pos.X, (TITLE_VERTICAL_MARGIN - 1) + i, title_name[i], Color::LIGHT_SKY_BLUE, Color::BLACK);
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
	short rendering_start_pos_y = static_cast<short>(renderer.get_max_height() - choices.size() - TITLE_VERTICAL_MARGIN);

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
	short rendering_start_pos_x = static_cast<short>((renderer.get_max_width() - choices_strings_max_len) * 0.5 - 1);
	short rendering_start_pos_y = static_cast<short>((renderer.get_max_height() - choices.size()) * 0.5);

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