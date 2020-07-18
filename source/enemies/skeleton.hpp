#pragma once

#include "../object.hpp"

class Skeleton: public Object 
{
public:
	Skeleton(float x, float y, bool flip);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int state;
	int direction;
	int timer;
	float hsp;
	float imageIndex;
};

class Bone: public Object 
{
public:
	Bone(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	float hsp, vsp;
	float imageIndex;
};