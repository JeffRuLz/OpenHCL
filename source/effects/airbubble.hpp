#pragma once

#include "../object.hpp"

class AirBubble: public Object 
{
public:
	AirBubble(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	float z;
	float xstart;
	float imageIndex;
	int timer;
};