#include "ArrowKeyPressDetector.h"

#include <windows.h>

void ArrowKeyPressDetector::update()
{
	prev_press = curr_press;

	GetAsyncKeyState(VK_UP) ? curr_press.set(0, true) : curr_press.set(0, false);
	GetAsyncKeyState(VK_RIGHT) ? curr_press.set(1, true) : curr_press.set(1, false);
	GetAsyncKeyState(VK_DOWN) ? curr_press.set(2, true) : curr_press.set(2, false);
	GetAsyncKeyState(VK_LEFT) ? curr_press.set(3, true) : curr_press.set(3, false);

	std::bitset<4> diff = prev_press ^ curr_press;
	for (int i = 0; i < 4; i++)
		if (diff.test(i) && curr_press.test(i))
			pressed_keys.push(i);
}

std::bitset<4> ArrowKeyPressDetector::get_curr_press()
{
	return curr_press;
}

std::bitset<4> ArrowKeyPressDetector::get_prev_press()
{
	return prev_press;
}

uint8_t ArrowKeyPressDetector::get_last_pressed_key()
{
	while (!curr_press.test(pressed_keys.top()))
		pressed_keys.pop();
	return pressed_keys.top();
}