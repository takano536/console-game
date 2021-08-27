#include "get_selected_choices.h"

#include <array>
#include "FpsSustainer.h"

short get_strings_max_len(const std::vector<std::string, StlAllocator<std::string>>& strings)
{
	short ret = 0;
	for (const auto& str : strings)
		ret = max(static_cast<short>(str.size()), ret);
	return ret;
}

int get_selected_choices(Renderer& renderer, const std::vector<std::string, StlAllocator<std::string>>& choices, short x, short y)
{
	const short choices_num = static_cast<short>(choices.size());
	const short max_choices_string_len = get_strings_max_len(choices);

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

		// ���Ԃɗ]�T������Α҂�
		fps_sustainer.wait();
	}

	return curr_cursor;
}