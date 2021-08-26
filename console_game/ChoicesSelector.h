#pragma once

#include <array>
#include <vector>
#include <string>
#include <windows.h>
#include "StlAllocator.h"

class ChoicesSelector final
{
public:
	ChoicesSelector(const std::vector<std::string, StlAllocator<std::string>>& choices_strings, const std::array<COORD, 2>& cursor_rendering_pos);
	const std::vector<std::string, StlAllocator<std::string>>& get_choices_strings();
	const std::array<COORD, 2>& get_cursor_rendering_pos();
	void move_cursor();

private:
	short choices_num;
	std::vector<std::string, StlAllocator<std::string>> choices_strings;
	short choices_string_max_len;
	short curr_choice;
	std::array<COORD, 2> cursor_rendering_pos;
};