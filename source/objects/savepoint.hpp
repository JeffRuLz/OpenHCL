#pragma once

#include "../object.hpp"

class SavePoint: public Object 
{
public:
	SavePoint(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
};