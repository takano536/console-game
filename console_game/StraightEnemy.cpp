#include "StraightEnemy.h"

#include "Color.h"

StraightEnemy::StraightEnemy(short x, short y, bool is_upper_case)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = is_upper_case ? 'E' : 'e';
	foreground_color = Color::LIGHT_PULPLE;
	background_color = Color::BLACK;
	hp = 3;
	this->is_upper_case = is_upper_case;
	reverse = false;
}

void StraightEnemy::move()
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	short add = reverse ? -1 : 1;
	if (is_upper_case)
		curr_pos.X += add;
	else
		curr_pos.Y += add;
}

void StraightEnemy::set_reverse()
{
	reverse = !reverse;
}
