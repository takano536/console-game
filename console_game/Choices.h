#pragma once

#include <vector>
#include <string>
#include "StlAllocator.h"
#include "Renderer.h"

class Choices
{
public:
	Choices(std::vector<std::string, StlAllocator<std::string>> choices);
	int choices_loop(short vertical_margin, Renderer& renderer);
	short get_choices_num();

private:
	std::vector<std::string, StlAllocator<std::string>> choices;
	short get_strings_max_len();
};