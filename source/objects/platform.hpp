#pragma once

#include "../object.hpp"

class Platform: public Object 
{
public:
	Platform(float x, float y, int type, int arg1, int arg2, int arg3 = 0);

	void update();
	void draw(float subFrame, float depth);

private:
	int type;
	float speed;
	int distance;
	int direction;
	int timer;
	int breakTimer;
	bool shake;
	bool visible;
	int invisibleTime;
	int visibleTime;
	float xstart;
};