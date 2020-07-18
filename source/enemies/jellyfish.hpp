#pragma once

#include "../object.hpp"

class Jellyfish: public Object 
{
public:
	Jellyfish(float x, float y, int bobDelay);

	void update();
	void draw(float subFrame, float depth);

private:
	int state;
	float ang;
	float ystart;
	float speed;
	float imageIndex;
};