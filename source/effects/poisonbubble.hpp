#pragma once

#include "../object.hpp"

class PoisonBubble: public Object 
{
public:
	PoisonBubble(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
};