#pragma once

#include "../object.hpp"

class BatBoss: public Object 
{
public:
	BatBoss(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int flag;
	int timer;
	int bobRot;
	float hsp, vsp;
	float imageIndex;
	float ypos;
	bool stompNext;
};