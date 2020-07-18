#pragma once

#include "../object.hpp"

class Gas: public Object 
{
public:
	Gas(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	char state;
	int timer;
	float imageIndex;
};