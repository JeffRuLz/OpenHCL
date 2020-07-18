#pragma once

#include "../object.hpp"

class LightSwitch: public Object 
{
public:
	LightSwitch(float x, float y, int flag, int keyFlag);

	void update();
	void draw(float subFrame, float depth);

private:
	int keyFlag;
	int flag;
	bool lit;
	float imageIndex;
};