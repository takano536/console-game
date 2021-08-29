#include "title.h"

#include <string>
#include <vector>
#include <array>
#include <windows.h>
#include "StlAllocator.h"
#include "Renderer.h"
#include "FpsSustainer.h"
#include "AsciiArt.h"
#include "Choices.h"
#include "Color.h"

static constexpr short TITLE_VERTICAL_MARGIN = 7;

int title_loop()
{
	Renderer renderer;
	renderer.init_window();

	// タイトルデータ
	AsciiArt title_art("yokero!");

	// タイトルアニメーション
	COORD title_rendering_start_pos = {static_cast<short>((renderer.get_max_width() - title_art.strings_len) * 0.5 - 1), 0};
	for (int i = 0; i < title_art.line_num + TITLE_VERTICAL_MARGIN; i++)
	{
		if (i - title_art.line_num == 0)
			continue;
		renderer.init_screen_buffer();
		title_rendering_start_pos.Y = max(i - title_art.line_num, 0);
		for (int j = max(title_art.line_num - i - 1, 0), line_cnt = 0; j < title_art.line_num; j++, line_cnt++)
			renderer.set_string(title_rendering_start_pos.X, title_rendering_start_pos.Y + line_cnt, title_art.ascii_art[j], Color::LIGHT_SKY_BLUE, Color::BLACK);
		renderer.render();
		Sleep(100);
	}
	renderer.init_screen_buffer();
	title_rendering_start_pos.Y = TITLE_VERTICAL_MARGIN - 1;
	for (int i = 0; i < title_art.line_num; i++)
		renderer.set_string(title_rendering_start_pos.X, title_rendering_start_pos.Y + i, title_art.ascii_art[i], Color::LIGHT_SKY_BLUE, Color::BLACK);
	renderer.render();
	Sleep(100);

	// 選択肢データ
	Choices choices({"GAME START", "  OPTION  ", "   QUIT   "});

	// 選択肢の表示と取得
	return choices.choices_loop(TITLE_VERTICAL_MARGIN, renderer);
}

void option_loop()
{
	Renderer renderer;
	renderer.init_window();

	// 選択肢データ
	Choices choices({"MAP DATA FILE PATH", " PLAYER MOVEMENTS ", "    DIFFICULTY    ", "       BACK       "});

	bool is_continue = true;
	while (is_continue)
	{
		short selected_choices = choices.choices_loop(static_cast<short>(renderer.get_max_height() - choices.get_choices_num()) * 0.5, renderer);
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