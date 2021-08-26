#include "Player.h"
#include "Color.h"

Player::Player(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = '@';
	foreground_color = Color::LIGHT_GREEN;
	background_color = Color::BLACK;
	hp = 3;
	arrow_key.up = false;
	arrow_key.down = false;
	arrow_key.left = false;
	arrow_key.right = false;
}

void Player::move()
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	if (!arrow_key.left && GetAsyncKeyState(VK_LEFT))
		curr_pos.X--, arrow_key.left = true;
	else if (arrow_key.left && !GetAsyncKeyState(VK_LEFT))
		arrow_key.left = false;

	else if (!arrow_key.right && GetAsyncKeyState(VK_RIGHT))
		curr_pos.X++, arrow_key.right = true;
	else if (arrow_key.right && !GetAsyncKeyState(VK_RIGHT))
		arrow_key.right = false;

	else if (!arrow_key.down && GetAsyncKeyState(VK_DOWN))
		curr_pos.Y++, arrow_key.down = true;
	else if (arrow_key.down && !GetAsyncKeyState(VK_DOWN))
		arrow_key.down = false;

	else if (!arrow_key.up && GetAsyncKeyState(VK_UP))
		curr_pos.Y--, arrow_key.up = true;
	else if (arrow_key.up && !GetAsyncKeyState(VK_UP))
		arrow_key.up = false;
}