#pragma once

#include "../object.hpp"

class Wizard: public Object 
{
public:
	Wizard(float x, float y, int flag);

	void update();
	void draw(float subFrame, float depth);

private:
	int flag;
	int state;
	int timer;
	int counter;
	bool visible;
	float imageIndex;
};