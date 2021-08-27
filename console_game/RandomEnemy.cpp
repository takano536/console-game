#include "RandomEnemy.h"

#include "Color.h"

RandomEnemy::RandomEnemy(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = 'R';
	foreground_color = Color::PINK;
	background_color = Color::BLACK;
	hp = 3;
	direction = 0;
}

void RandomEnemy::move(uint64_t rand)
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	if (rand % 3)
		return;

	switch (rand % 4)
	{
		case 0:
			curr_pos.Y--;
			break;
		case 1:
			curr_pos.X++;
			break;
		case 2:
			curr_pos.Y++;
			break;
		case 3:
			curr_pos.X--;
			break;
		default:
			break;
	}
}

void RandomEnemy::undo()
{
	curr_pos.X = prev_pos.X;
	curr_pos.Y = prev_pos.Y;
}

void RandomEnemy::set_direction()
{
	direction = (direction + 1) % 4;
}