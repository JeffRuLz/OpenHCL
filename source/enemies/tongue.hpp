#pragma once

#include "../object.hpp"

class Tongue: public Object 
{
public:
	Tongue(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int timer;
	int direction;
	float imageIndex;
	float tongueImageIndex;
};