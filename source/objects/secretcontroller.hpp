#pragma once

#include "../object.hpp"

class SecretController: public Object 
{
public:
	SecretController(int flag, int count, int* flags);

	void update();
	void draw(float subFrame, float depth);

private:
	int flags[20];
	int numOfFlags;
	bool check;
};