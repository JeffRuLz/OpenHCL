#pragma once

#include "../object.hpp"

class HydraHead: public Object 
{
public:
	HydraHead(Object* owner = nullptr, int position = 0);

	void update();
	void draw(float subFrame, float depth);

	void updateNeck(float inx, float iny, int index = -1, float rot = -1, float* outx = nullptr, float* outy = nullptr);
	void setState(int state, int timer);

private:
	int hp;
	int state;
	int position;
	int blinkTimer;
	int timer;
	int counter;
	float imageIndex;
	float neckRot[2];
	float neckPosX[7];
	float neckPosY[7];
	Object* owner;
};

class Hydra: public Object 
{
public:
	Hydra(float x, float y, int flag);

	void update();
	void draw(float subFrame, float depth);

private:
	int hp;
	int flag;
	int state;
	int timer;
	int counter;
	int blinkTimer;
	bool onground;
	bool headless;
	float hsp, vsp;
	float imageIndex;
	HydraHead head[4];
};

class HydraGoop: public Object 
{
public:
	HydraGoop(float x, float y);

	void update();
	void draw(float subFrame, float depth);

private:
	bool bounce;
	float hsp, vsp;
};

class HydraRock: public Object 
{
public:
	HydraRock();

	void update();
	void draw(float subFrame, float depth);

private:
	bool bounce;
	float vsp;
	float imageIndex;
};

class HydraBolt: public Object 
{
public:
	HydraBolt(float x, float y);

	void update();
	void draw(float subFrame, float depth);
	
private:
	int state;
	int timer;
	float hsp, vsp;
};