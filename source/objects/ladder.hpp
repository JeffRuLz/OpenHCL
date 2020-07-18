#pragma once

#include "../object.hpp"

class Ladder: public Object 
{
public:
	Ladder(float x, float y, int height);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int height;
	bool set;
};