#pragma once

#include <cstdint>
#include <windows.h>
#include "Color.h"

class Object
{
public:
	virtual ~Object() {}
	COORD get_curr_pos() const;
	COORD get_prev_pos() const;
	char get_graphic() const;
	Color get_foreground_color() const;
	Color get_background_color() const;

protected:
	COORD curr_pos;
	COORD prev_pos;
	char graphic;
	Color foreground_color;
	Color background_color;
	int16_t hp;
};

bool is_collided(Object a, Object b);