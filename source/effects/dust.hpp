#pragma once

#include "../object.hpp"

class Dust: public Object 
{
public:
	Dust(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
};