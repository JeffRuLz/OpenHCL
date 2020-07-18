#pragma once

#include "../object.hpp"

class Boar: public Object
{
public:
	Boar(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int timer;
	int direction;
	float imageIndex;
	float hsp;
};