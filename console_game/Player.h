#pragma once

#include "Creature.h"
#include "ArrowKey.h"

class Player : public Creature
{
public:
	Player(short x, short y);
	void move();

private:
	ArrowKey arrow_key;
};