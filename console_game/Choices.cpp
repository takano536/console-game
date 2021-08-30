#include "Choices.h"

#include <array>
#include "FpsSustainer.h"
#include "KeyPressDetector.h"

Choices::Choices(std::vector<std::string, StlAllocator<std::string>> choices)
{
	this->choices = choices;
}

int Choices::choices_loop(short vertical_margin, Renderer& renderer)
{
	short rendering_start_pos_x = static_cast<short>((renderer.get_max_width() - get_strings_max_len()) * 0.5 - 1);
	short rendering_start_pos_y = static_cast<short>(renderer.get_max_height() - choices.size() - vertical_margin);

	const short choices_num = static_cast<short>(choices.size());
	const short max_choices_string_len = get_strings_max_len();

	COORD choices_rendering_start_pos = {rendering_start_pos_x, rendering_start_pos_y};
	for (int i = 0; i < choices_num; i++)
		renderer.set_string(choices_rendering_start_pos.X, choices_rendering_start_pos.Y + i, choices[i], Color::WHITE, Color::BLACK);

	std::string message = "ARROW KEY: MOVE   ENTER KEY: SELECT";  // 操作説明
	renderer.set_string((renderer.get_max_width() - 1) - static_cast<short>(message.size()) - 1, renderer.get_max_height() - 2, message, Color::WHITE, Color::BLACK);

	short curr_cursor = 0;  // 選択肢番号
	std::array<COORD, 2> cursor_rendering_pos = {
	    COORD{static_cast<short>(choices_rendering_start_pos.X - 3), choices_rendering_start_pos.Y},
	    COORD{static_cast<short>(choices_rendering_start_pos.X + max_choices_string_len + 1), choices_rendering_start_pos.Y},
	};
	FpsSustainer fps_sustainer;
	KeyPressDetector key_detector;
	bool is_selected = false;
	while (!is_selected)
	{
		fps_sustainer.init_start_time();
		key_detector.update();

		if (key_detector.get_curr_press().test(0) && !key_detector.get_prev_press().test(0))  // 上キー
			curr_cursor = curr_cursor - 1 < 0 ? choices_num - 1 : curr_cursor - 1;
		else if (key_detector.get_curr_press().test(2) && !key_detector.get_prev_press().test(2))  // 下キー
			curr_cursor = (curr_cursor + 1) % choices_num;
		else if (key_detector.get_curr_press().test(4) && !key_detector.get_prev_press().test(4))  // enter キー
			is_selected = true;

		// カーソルを描画
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

short Choices::get_choices_num()
{
	return choices.size();
}

short Choices::get_strings_max_len()
{
	short ret = 0;
	for (const auto& str : choices)
		ret = max(static_cast<short>(str.size()), ret);
	return ret;
}