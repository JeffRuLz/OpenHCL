#pragma once

#include "../object.hpp"

class Gate: public Object 
{
public:
	Gate(float x, float y, int color, int keyFlag);

	void update();
	void draw(float subFrame, float depth);

private:
	bool opened;
	bool red;
	int keyFlag;
	float imageIndex;
};