#pragma once

#include "../object.hpp"

class Splash: public Object 
{
public:
	Splash(float x, float y, float hsp, float vsp, bool lava);

	void update();
	void draw(float subFrame, float depth);

private:
	bool lava;
	float z;
	float hsp, vsp;
	float imageIndex;
};

void createSplash(float x, float y, bool lava = false);