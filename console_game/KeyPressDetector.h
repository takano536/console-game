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
	std::bitset<6> get_curr_press();
	std::bitset<6> get_prev_press();
	uint8_t get_last_pressed_key();

private:
	std::bitset<6> curr_press;  // 0:ª  1:¨  2:«  3:©  4:enter  5:escape
	std::bitset<6> prev_press;
	std::stack<uint8_t> pressed_keys;
};