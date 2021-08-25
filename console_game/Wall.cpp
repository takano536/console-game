#include "Wall.h"
#include "Color.h"

Wall::Wall(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = '#';
	foreground_color = static_cast<WORD>(Color::WHITE);
	background_color = static_cast<WORD>(Color::BLACK);
	hp = -1;
}