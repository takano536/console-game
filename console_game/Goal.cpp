#include "Goal.h"

#include "Color.h"

Goal::Goal(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = 'G';
	foreground_color = Color::WHITE;
	background_color = Color::BLUE;
	hp = -1;
}