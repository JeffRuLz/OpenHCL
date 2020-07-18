#pragma once

#include "../object.hpp"

class BossMusicController: public Object 
{
public:
	BossMusicController(int flag);

	void update();

private:
	int flag;
	bool check;
};