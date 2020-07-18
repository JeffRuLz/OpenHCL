#pragma once

#include "../object.hpp"

class Golem: public Object 
{
public:
	Golem(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int direction;
	float imageIndex;
};