#pragma once

#include "../object.hpp"

class Pendulum: public Object
{
public:
	Pendulum(float x, float y, int offset);

	void update();
	void draw(float subFrame, float depth);

private:
	float ang[2];
	int rot;
};