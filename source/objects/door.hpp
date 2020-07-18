#pragma once

#include "../object.hpp"

class Door: public Object 
{
public:
	Door(float x, float y, int keyFlag, int openFlag, int level, int room, int warpx, int warpy);

	void update();
	void draw(float subFrame, float depth);
	
private:
	int keyFlag;
	int openFlag;
	int level;
	int room;
	int warpx;
	int warpy;
	bool opened;
};