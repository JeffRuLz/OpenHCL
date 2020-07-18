#pragma once

#include "../object.hpp"

class Labyrush: public Object 
{
public:
	Labyrush(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int timer;
	int counter;
	int direction;
	float hsp, vsp;
	float imageIndex;
};