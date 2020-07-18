#pragma once

#include "../object.hpp"

class FloorButton: public Object 
{
public:
	FloorButton(float x, float y, int flag);

	void update();
	void draw(float subFrame, float depth);

private:
	bool pressed;
	int flag;
};