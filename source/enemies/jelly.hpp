#pragma once

#include "../object.hpp"

class Jelly: public Object
{
public:
	Jelly(float x, float y, int type = 0, int delay = 0);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	int hp;
	float hsp, vsp;
	float imageIndex;
	int type;
	int timer;
	int counter;
	int state;
	bool onground;
};