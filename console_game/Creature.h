#pragma once

#include "Object.h"

class Creature : public Object
{
public:
	void undo();  // 座標を undo する
};