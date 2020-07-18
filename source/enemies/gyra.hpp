#pragma once

#include "../object.hpp"

class Gyra: public Object 
{
public:
	Gyra(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	int state;
	int timer;
	int blinkTimer;
	int counter;
	float imageIndex;
	float ang;
	float targx, targy;
	float bodyx[120];
	float bodyy[120];
};