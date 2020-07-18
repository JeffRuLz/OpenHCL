#pragma once

#include "../object.hpp"

class Kappa: public Object 
{
public:
	Kappa(float x, float y, int subtype, int jumpHeight, int jumpDelay, int startDelay);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int type;
	int ang;
	int timer;
	int delay;
	float ystart;
	float imageIndex;
	float hsp, vsp;
	float jumpHeight;
	bool jumpDown;
};