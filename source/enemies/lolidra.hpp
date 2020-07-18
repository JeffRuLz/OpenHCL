#pragma once

#include "../object.hpp"

class Lolidra: public Object 
{
public:
	Lolidra(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	int state;
	int nextState;
	int timer;
	int flashTimer;
	int counter;
	int hoverRot;
	bool visible;
	float imageIndex;
	float ypos;
};

class Minion: public Object 
{
public:
	Minion(float x, float y);
	~Minion();

	void update();
	void draw(float subFrame, float depth);

private:
	int state;
	int timer;
	int direction;
	float speed;
	float imageIndex;
	float ypos;
};