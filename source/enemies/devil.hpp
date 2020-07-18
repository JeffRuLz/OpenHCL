#pragma once

#include "../object.hpp"

class Devil: public Object 
{
public:
	Devil(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	int state;
	int timer;
	int blinkTimer;
	int counter;
	int direction;
	float imageIndex;
	float hsp, vsp;

	float bobSpeed;
	float bobRotSpeed;
	float bobRot;

	float tailAngle;
	float tailSpeed;
	float tailRot[2];
};

class Orb: public Object 
{
public:
	Orb(float x, float y, float ang);

	void update();
	void draw(float subFrame, float depth);

private:
	float hsp, vsp;
	float imageIndex;
};