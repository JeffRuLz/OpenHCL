#pragma once

#include "../object.hpp"

class Crown: public Object 
{
public:
	Crown(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	int rot;
	int timer;
	float ystart;
	float imageIndex;
};