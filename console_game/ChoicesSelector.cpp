#include "ChoicesSelector.h"
#include <windows.h>

short get_string_max_len(const std::vector<std::string, StlAllocator<std::string>>& strings)
{
	short ret = 0;
	for (const auto& s : strings)
		ret = max(s.size(), ret);
	return ret;
}

ChoicesSelector::ChoicesSelector(const std::vector<std::string, StlAllocator<std::string>>& choices_strings, const std::array<COORD, 2>& cursor_rendering_pos)
{
	choices_num = choices_strings.size();
	this->choices_strings.resize(choices_num);
	this->choices_strings = choices_strings;
	choices_string_max_len = get_string_max_len(choices_strings);
	curr_choice = 0;
	this->cursor_rendering_pos = cursor_rendering_pos;
}

const std::vector<std::string, StlAllocator<std::string>>& ChoicesSelector::get_choices_strings()
{
	return choices_strings;
}

const std::array<COORD, 2>& ChoicesSelector::get_cursor_rendering_pos()
{
	return cursor_rendering_pos;
}

void ChoicesSelector::move_cursor()
{
	if (GetAsyncKeyState(VK_DOWN) & 1)
		curr_choice = (curr_choice + 1) % choices_num;
	else if (GetAsyncKeyState(VK_UP) & 1)
		curr_choice = curr_choice - 1 < 0 ? choices_num - 1 : curr_choice - 1;
}