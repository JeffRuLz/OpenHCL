#pragma once

#include "../object.hpp"

class Ghost: public Object 
{
public:
	Ghost(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	int state;
	int timer;
	int ang;
	float imageIndex;
	int rot;
	float yoffset[2];
};