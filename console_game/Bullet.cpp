#include "Bullet.h"

#include "Color.h"

Bullet::Bullet(short x, short y, uint8_t direction)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = 'o';
	foreground_color = Color::YELLOW;
	background_color = Color::BLACK;
	hp = 1;
	this->direction = direction;
}

void Bullet::move()
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	switch (direction)
	{
		case 0:
			curr_pos.X++;
			break;
		case 1:
			curr_pos.X--;
			break;
		default:
			break;
	}
}