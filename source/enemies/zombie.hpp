#pragma once

#include "../object.hpp"

class Zombie: public Object 
{
public:
	Zombie(float x, float y, int type);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int type;
	int state;
	int direction;
	float vsp;
	float imageIndex;
	bool onGround;
};