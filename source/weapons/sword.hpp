#pragma once

#include "../object.hpp"

class Sword: public Object
{
public:
	Sword(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	float imageIndex;
	Rectangle mask;
};