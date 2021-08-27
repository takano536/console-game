#pragma once

#include <cstdint>
#include "Object.h"

class RandomEnemy : public Object
{
public:
	RandomEnemy(short x, short y);
	void move(uint64_t rand);
	void undo();
	void set_direction();

private:
	int direction;  // 0:ª 1:¨ 2:« 3:©
};