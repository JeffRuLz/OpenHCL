#pragma once

#include "../object.hpp"

class GuruGuru: public Object 
{
public:
	GuruGuru(float x, float y, int quarterTurns, bool counterClockwise);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int direction;
	int timer;
	bool clockwise;
	float imageIndex;
};