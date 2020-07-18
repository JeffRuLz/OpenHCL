#pragma once

#include "../object.hpp"

class Axe: public Object
{
public:
	Axe(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	float imageIndex;
	float hsp, vsp;
	Circle mask;

	void destroyEvent();
};