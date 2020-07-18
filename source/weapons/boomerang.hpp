#pragma once

#include "../object.hpp"

class Boomerang: public Object 
{
public:
	Boomerang(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	float hsp;
	float imageIndex;
	int timer;
	int dir;
	Circle mask;
};