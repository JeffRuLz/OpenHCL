#pragma once

#include "../object.hpp"

class Dodo: public Object 
{
public:
	Dodo(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int direction;
	int timer;
	int flag;
	float hsp, vsp;
	float imageIndex;
	bool onGround;
	bool chaseNext;
	bool highJumpNext;
};