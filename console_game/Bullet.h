#pragma once

#include "Creature.h"

#include <cstdint>

class Bullet : public Creature
{
public:
	Bullet(short x, short y, uint8_t direction);
	void move();

private:
	uint8_t direction;  // 0:Å® 1:Å©
};