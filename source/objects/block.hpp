#pragma once

#include "../object.hpp"

class Block: public Object 
{
public:
	Block(float x, float y, int subtype = 0, int tile = 0);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int tile;
	int type;
};