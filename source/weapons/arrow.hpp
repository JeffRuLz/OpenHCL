#pragma once

#include "../object.hpp"

class Arrow: public Object
{
public:
	Arrow(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	float hsp;
	Rectangle mask;

	void destroyEvent();
};