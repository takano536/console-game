#pragma once

#include "Object.h"

class Creature : public Object
{
public:
	virtual void move() = 0;
	void undo();  // À•W‚ğ undo ‚·‚é
};