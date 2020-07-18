#pragma once

#include "../object.hpp"

class Chest: public Object 
{
public:
	Chest(float x, float y, bool isKey, int flag);

	void create();
	void update();
	void draw(float subFrame, float depth);

private:
	bool isKey;
	int timer;
	int flag;
};