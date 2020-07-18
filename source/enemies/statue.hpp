#pragma once

#include "../object.hpp"

class Statue: public Object 
{
public:
	Statue(float x, float y, int type, bool flip, int offset, int timer);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
	int hp;
	int timer;
	int type;
	int state;
	int direction;
	int waitTime;
	int counter;
};

class Bullet: public Object 
{
public:
	Bullet(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);
	
private:
	int direction;
	float imageIndex;
};

class Laser: public Object 
{
public:
	Laser(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	int direction;
	float imageIndex;
};

class EnemyFireBall: public Object
{
public:
	EnemyFireBall(float x, float y, float ang, float speed);

	void update();
	void draw(float subFrame, float depth);

private:
	float hsp, vsp;
	float imageIndex;
};

class Air: public Object 
{
public:
	Air(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
};

class Boulder: public Object 
{
public:
	Boulder(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
	float hsp, vsp;
};

class Fire: public Object 
{
public:
	Fire(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth);

private:
	int timer;
	int direction;
	float imageIndex;
};