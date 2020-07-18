#pragma once

#include "../object.hpp"

class Garm: public Object 
{
public:
	Garm(float x, float y, int flag);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int timer;
	int counter;
	int jumpCounter;
	float imageIndex;
	float hsp, vsp;
};

class GarmRock: public Object 
{
public:
	GarmRock(float x, float y, float hsp, float vsp);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int counter;
	float imageIndex;
	float hsp, vsp;
};