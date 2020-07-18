#pragma once

#include "../object.hpp"

class Shock: public Object 
{
public:
	Shock(float x, float y);

	void update();
	void draw(float subFrame, float depth);
	
private:
	float imageIndex;
};