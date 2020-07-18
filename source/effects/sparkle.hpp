#pragma once

#include "../object.hpp"

class Sparkle: public Object 
{
public:
	Sparkle(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
};