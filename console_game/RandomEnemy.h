#pragma once

#include <cstdint>
#include "Creature.h"

class RandomEnemy : public Creature
{
public:
	RandomEnemy(short x, short y);
	void move(uint64_t rand);
	void set_direction();

private:
	int direction;  // 0:ª 1:¨ 2:« 3:©
};