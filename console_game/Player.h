#pragma once

#include "Creature.h"
#include <bitset>

class Player : public Creature
{
public:
	Player(short x, short y);
	void move(uint8_t last_pressed_key);
};