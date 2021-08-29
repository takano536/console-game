#include "KeyPressDetector.h"

#include <windows.h>

KeyPressDetector::KeyPressDetector()
{
	GetAsyncKeyState(VK_UP) ? curr_press.set(0, true) : curr_press.set(0, false);
	GetAsyncKeyState(VK_RIGHT) ? curr_press.set(1, true) : curr_press.set(1, false);
	GetAsyncKeyState(VK_DOWN) ? curr_press.set(2, true) : curr_press.set(2, false);
	GetAsyncKeyState(VK_LEFT) ? curr_press.set(3, true) : curr_press.set(3, false);
	GetAsyncKeyState(VK_RETURN) ? curr_press.set(4, true) : curr_press.set(4, false);
	GetAsyncKeyState(VK_ESCAPE) ? curr_press.set(5, true) : curr_press.set(5, false);

	prev_press = curr_press;

	for (int i = 0; i < 5; i++)
		if (curr_press.test(i))
			pressed_keys.push(i);
}

void KeyPressDetector::update()
{
	prev_press = curr_press;

	GetAsyncKeyState(VK_UP) ? curr_press.set(0, true) : curr_press.set(0, false);
	GetAsyncKeyState(VK_RIGHT) ? curr_press.set(1, true) : curr_press.set(1, false);
	GetAsyncKeyState(VK_DOWN) ? curr_press.set(2, true) : curr_press.set(2, false);
	GetAsyncKeyState(VK_LEFT) ? curr_press.set(3, true) : curr_press.set(3, false);
	GetAsyncKeyState(VK_RETURN) ? curr_press.set(4, true) : curr_press.set(4, false);
	GetAsyncKeyState(VK_ESCAPE) ? curr_press.set(5, true) : curr_press.set(5, false);

	std::bitset<6> diff = prev_press ^ curr_press;
	for (int i = 0; i < 6; i++)
		if (diff.test(i) && curr_press.test(i))
			pressed_keys.push(i);
}

std::bitset<6> KeyPressDetector::get_curr_press()
{
	return curr_press;
}

std::bitset<6> KeyPressDetector::get_prev_press()
{
	return prev_press;
}

uint8_t KeyPressDetector::get_last_pressed_key()
{
	while (!curr_press.test(pressed_keys.top()))
		pressed_keys.pop();
	return pressed_keys.top();
}