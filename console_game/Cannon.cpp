#include "Cannon.h"

#include <set>
#include "Color.h"
#include "Renderer.h"

Cannon::Cannon(short x, short y)
{
	curr_pos = COORD{x, y};
	prev_pos = COORD{x, y};
	graphic = '-';
	foreground_color = Color::LIGHT_YELLOW;
	background_color = Color::BLACK;
	hp = -1;
}