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
	COORD curr_pos;  // 現在のフレームでの位置
	COORD prev_pos;  // 前のフレームでの位置
	char graphic;
	Color foreground_color;
	Color background_color;
	int16_t hp;  // 時間が足りなくて使わなかった
};

bool is_collided(Object a, Object b);