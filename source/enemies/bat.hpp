#pragma once

#include "../object.hpp"

class Bat: public Object 
{
public:
	Bat(float x, float y, char type);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float xstart, ystart;
	float imageIndex;
	char state;
	char type;
	int direction;
	bool flyback;
	int timer;
	int angle;	
};