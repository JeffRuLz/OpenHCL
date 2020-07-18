#pragma once

#include "../object.hpp"

class Pebble: public Object 
{
public:
	Pebble(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	float z;
	float hsp, vsp;
	float imageIndex;
	int timer;
};