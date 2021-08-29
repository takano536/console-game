#pragma once

#include <string>
#include <array>
#include <windows.h>
#include "StlAllocator.h"

struct AsciiArt final
{
	AsciiArt(std::string str);

	std::array<std::string, 7> ascii_art;
	const short line_num = 7;
	short strings_len;
};