#pragma once

#include "../object.hpp"

class CrushTrap: public Object 
{
public:
	CrushTrap(float x, float y, int type, bool flip, int waitTime, int delay);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int type;
	int state;
	int timer;
	int waitTime;
	int direction;
	int blinkTimer;
	float speed;
	float imageIndex;
	float startx, starty;
};