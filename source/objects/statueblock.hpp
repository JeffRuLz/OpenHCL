#pragma once

#include "../object.hpp"

class StatueBlock: public Object 
{
public:
	StatueBlock(float x, float y, int type, int keyFlag);

	void update();
	void draw(float subFrame, float depth);
	
private:
	int hp;
	int type;
	int keyFlag;
	int blinkTimer;
};