#include "Enemy.h"
#include "Color.h"

Enemy::Enemy(short x, short y, bool is_upper_case)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = is_upper_case ? 'E' : 'e';
	foreground_color = is_upper_case ? static_cast<WORD>(Color::PURPLE) : static_cast<WORD>(Color::LIGHT_PULPLE);
	background_color = static_cast<WORD>(Color::BLACK);
	hp = 3;
	this->is_upper_case = is_upper_case;
	reverse = false;
}

void Enemy::move()
{
	prev_pos.X = curr_pos.X;
	prev_pos.Y = curr_pos.Y;

	short add = reverse ? -1 : 1;
	if (is_upper_case)
		curr_pos.X += add;
	else
		curr_pos.Y += add;
}

void Enemy::set_reverse()
{
	reverse = !reverse;
}
