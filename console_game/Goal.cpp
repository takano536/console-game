#include "goal.h"
#include "Color.h"

Goal::Goal(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = 'G';
	foreground_color = static_cast<WORD>(Color::BLACK);
	background_color = static_cast<WORD>(Color::LIGHT_SKY_BLUE);
	hp = -1;
}