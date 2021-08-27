#include "Wall.h"

#include "Color.h"

Wall::Wall(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = '#';
	foreground_color = Color::WHITE;
	background_color = Color::BLACK;
	hp = -1;
}