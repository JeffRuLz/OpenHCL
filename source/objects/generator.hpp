#pragma once

#include "../object.hpp"

class Generator: public Object 
{
public:
	Generator(float x, float y, int flag);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	float imageIndex;
};


class ElectricGate: public Object 
{
public:
	ElectricGate(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	float imageIndex;
};