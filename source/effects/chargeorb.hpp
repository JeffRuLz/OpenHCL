#pragma once

#include "../object.hpp"

class ChargeOrb: public Object 
{
public:
	ChargeOrb();

	void update();
	void draw(float subFrame, float depth);

private:
	int timer;
	int ang;
	float imageIndex;
};