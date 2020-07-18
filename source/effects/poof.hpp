#pragma once

#include "../object.hpp"

class Poof: public Object 
{
public:
	Poof(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
};