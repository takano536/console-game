#pragma once

#include <bitset>
#include <cstdint>
#include <stack>
#include "StlAllocator.h"

class ArrowKeyPressDetector final
{
public:
	void update();
	std::bitset<4> get_curr_press();
	std::bitset<4> get_prev_press();
	uint8_t get_last_pressed_key();

private:
	std::bitset<4> curr_press;  // 0:ª 1:¨ 2:« 3:©
	std::bitset<4> prev_press;
	std::stack<uint8_t> pressed_keys;
};