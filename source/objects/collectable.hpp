#pragma once

#include "../object.hpp"

void dropCollectable(float x, float y);

class Collectable: public Object 
{
public:
	Collectable(float x, float y, char type);

	void update();
	void draw(float subFrame, float depth);

private:
	char type;
	float vsp;
	int counter;
	int timer;
	bool firstFrame;
};