#pragma once

#include "../object.hpp"

class Worm: public Object 
{
public:
	Worm(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
	float hsp, vsp;
};