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
}

void Player::move(uint8_t last_pressed_key)
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	switch (last_pressed_key)
	{
		case 0:  // è„
			curr_pos.Y--;
			break;
		case 1:  // âE
			curr_pos.X++;
			break;
		case 2:  // â∫
			curr_pos.Y++;
			break;
		case 3:  // ç∂
			curr_pos.X--;
			break;
		defaut:
			break;
	}
}