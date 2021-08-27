#pragma once

#include <vector>
#include <string>
#include "StlAllocator.h"
#include "Renderer.h"

short get_strings_max_len(const std::vector<std::string, StlAllocator<std::string>>& strings);
int get_selected_choices(Renderer& renderer, const std::vector<std::string, StlAllocator<std::string>>& choices, short x, short y);