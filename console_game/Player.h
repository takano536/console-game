#pragma once

#include "Creature.h"
#include "ArrowKey.h"

class Player : public Creature
{
public:
	Player(short x, short y);
	void move();
	void update_arrow_key_flag();
	bool is_long_press();

private:
	ArrowKey arrow_key;
};