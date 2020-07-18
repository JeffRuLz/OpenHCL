#pragma once

#include "../object.hpp"

class Piranha: public Object 
{
public:
	Piranha(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
	int direction;
	float speed;
	int state;
	float xstart;
};