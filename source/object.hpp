#pragma once

#include "collision.hpp"

class Object
{
public:
	Object(float x = 0, float y = 0)
	{
		this->x = x;
		this->y = y;

		prevx = this->x;
		prevy = this->y;

		dead = false;
		solid = false;

		layer = 0;
		direction = 1;
		hitTimer = 0;

		spawnFlag = 0;
		despawnFlag = 0;
		killFlag = 0;
	};

	virtual ~Object() {};

	//
	virtual void create() {};
	virtual void update()
	{
		prevx = x;
		prevy = y;

		if (hitTimer > 0)
			hitTimer -= 1;
	};
	virtual void draw(float subFrame = 1.f, float depth = 0.f) {};

	//
	virtual bool collide(Rectangle r, Object* obj = nullptr) { return false; };
	virtual bool collide(Circle c, Object* obj = nullptr) { return false; };

	//
	bool isDead() { return dead; };
	bool isSolid() { return solid; };
	int getLayer() { return layer; };

	void warpX(float n) { x = n; prevx = x; };
	void warpY(float n) { y = n; prevy = y; };

	//
	float getX() { return x; };
	void setX(float n) { x = n; };

	float getY() { return y; };
	void setY(float n) { y = n; };

	float getPrevX() { return prevx; };

	float getPrevY() { return prevy; };

	int getDirection() { return direction; };
	void setDirection(int n) { if (n != 0) { direction = ((n > 0)? 1: -1); } };

	int getHitTimer() { return hitTimer; };
	void setHitTimer(int n) { hitTimer = n; };

	int getSpawnFlag() { return spawnFlag; };
	void setSpawnFlag(int f) { spawnFlag = f; };

	int getDespawnFlag() { return despawnFlag; };
	void setDespawnFlag(int f);

	int getKillFlag() { return killFlag; };
	void setKillFlag(int f) { killFlag = f; };

protected:
	float x, y;
	float prevx, prevy;
	bool dead;
	bool solid;
	int layer;
	int direction;
	int hitTimer;
	int spawnFlag;
	int despawnFlag;
	int killFlag;
};

class ObjectList
{
public:
	ObjectList();
	~ObjectList();

	void update();
	void draw(float subFrame, float depth = 0.f, int layer = -1);

	Object* add(Object* o);
	void clear();
	int count();

	Object* next(Object* o = nullptr);

	Object* collide(Rectangle r, Object* obj = nullptr);
	Object* collide(Circle c, Object* obj = nullptr);

private:
	Object* list[100];
	int size;
};