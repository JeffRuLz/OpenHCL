#pragma once

#include "../object.hpp"

class Knight: public Object 
{
public:
	Knight(float x, float y, char type);

	void update();
	void draw(float subFrame, float depth = 0.f);

private:
	int hp;
	int type;
	char state;
	float imageIndex;
	int timer;
	int blinkTimer;
	int direction;
};

class Goop: public Object 
{
public:
	Goop(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	float hsp;
	float imageIndex;
};

class EnemyBoomerang: public Object 
{
public:
	EnemyBoomerang(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	int direction;
	float xstart;
	float hsp;
	float imageIndex;
};

class Pumpkin: public Object 
{
public:
	Pumpkin(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	bool explosion;
	float hsp, vsp;
	float imageIndex;
};