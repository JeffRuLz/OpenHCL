#pragma once

#include "../object.hpp"

class Fireball: public Object 
{
public:
	Fireball(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	float hsp, vsp;
	float imageIndex;
	char counter;
	Circle mask;
};