#pragma once

#include "Creature.h"

class Enemy : public Creature
{
public:
	Enemy(short x, short y, bool is_upper_case);
	void move();
	void set_reverse();

private:
	bool is_upper_case;
	bool reverse;
};