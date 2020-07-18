#pragma once

#include "../object.hpp"

void rockSmashEffect(float x, float y);

class BrokenRock: public Object 
{
public:
	BrokenRock(float x, float y, float hsp, float vsp);

	void update();
	void draw(float subFrame, float depth);

private:
	float hsp, vsp;
	float z;
	float imageIndex;
	int timer;
	bool big;
};