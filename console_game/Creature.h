#pragma once

#include "Object.h"

class Creature : public Object
{
public:
	virtual void move() = 0;
	void undo();  // 座標を undo する
};