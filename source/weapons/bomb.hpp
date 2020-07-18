#pragma once

#include "../object.hpp"

class Bomb: public Object 
{
public:
	Bomb(float x, float y, int direction);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool collide(Rectangle r, Object* obj = nullptr);
	bool collide(Circle c, Object* obj = nullptr);

private:
	char state;
	float hsp, vsp;
	float imageIndex;
	char counter;
	Rectangle mask;
};