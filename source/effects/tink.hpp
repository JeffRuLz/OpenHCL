#pragma once

#include "../object.hpp"

class Tink: public Object 
{
public:
	Tink(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	float imageIndex;
};