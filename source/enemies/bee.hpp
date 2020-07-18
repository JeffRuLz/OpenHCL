#pragma once

#include "../object.hpp"

class Bee: public Object 
{
public:
	Bee(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	int state;
	int timer;
	int direction;
	float imageIndex;
	float hsp, vsp;
	float xstart;
	float ystart;
	float hoverRot;
};