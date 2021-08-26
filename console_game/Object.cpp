#include "Object.h"
#include "Color.h"

COORD Object::get_curr_pos() const
{
	return curr_pos;
}

COORD Object::get_prev_pos() const
{
	return prev_pos;
}

char Object::get_graphic() const
{
	return graphic;
}

Color Object::get_foreground_color() const
{
	return foreground_color;
}
Color Object::get_background_color() const
{
	return background_color;
}

bool is_collided(Object a, Object b)
{
	return a.get_curr_pos().X == b.get_curr_pos().X && a.get_curr_pos().Y == b.get_curr_pos().Y;
}