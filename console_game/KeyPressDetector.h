#pragma once

#include <bitset>
#include <cstdint>
#include <stack>
#include "StlAllocator.h"

class KeyPressDetector final
{
public:
	KeyPressDetector();
	void update();
	std::bitset<5> get_curr_press();
	std::bitset<5> get_prev_press();
	uint8_t get_last_pressed_key();

private:
	std::bitset<5> curr_press;  // 0:ª 1:¨ 2:« 3:© 4:enter
	std::bitset<5> prev_press;
	std::stack<uint8_t> pressed_keys;
};