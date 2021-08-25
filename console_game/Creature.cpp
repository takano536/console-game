#include "Creature.h"

void Creature::undo()
{
	curr_pos.X = prev_pos.X;
	curr_pos.Y = prev_pos.Y;
}