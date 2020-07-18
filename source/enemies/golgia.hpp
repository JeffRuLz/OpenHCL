#pragma once

#include "../object.hpp"

class Golgia: public Object 
{
public:
	Golgia(float x, float y, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	int state;
	int rollState;
	int timer;
	int counter;
	float imageIndex;
	float hsp, vsp;
};

class ElectricOrb: public Object 
{
public:
	ElectricOrb(float x, float y, float ang);

	void update();
	void draw(float subFrame, float depth);
	
private:
	float hsp, vsp;
	float imageIndex;
};