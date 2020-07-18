#pragma once

#include "../object.hpp"

class Lava: public Object 
{
public:
	Lava(float x, float y, int type);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
	int type;
};