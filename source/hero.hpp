#pragma once

#include "object.hpp"

enum HeroState
{
	HERO_STATE_NORMAL,
	HERO_STATE_SLASH,
	HERO_STATE_HIT,
	HERO_STATE_LADDER,
	HERO_STATE_STONE,
	HERO_STATE_CHARGE,
	HERO_STATE_ITEM_GET,
	HERO_STATE_QUAKE,
	HERO_STATE_DEATH,
	HERO_STATE_DOOR
};

class Hero: public Object
{
public:
	Hero(float x, float y);

	void update();
	void draw(float subFrame, float depth = 0.f);

	bool canHit();
	bool hit(Rectangle mask, int damage);
	bool hit(Circle mask, int damage);

	bool collide(Rectangle r);
	bool collide(Circle c);

	//status effects
	void kill();
	void stun();
	void petrify();
	void poison();

	void changeWeapon();

	Rectangle getMask();
	Rectangle getShieldMask();	

	//
	HeroState getState() { return state; };
	void setState(HeroState hs);

	float getHsp() { return hsp; };
	void setHsp(float n) { hsp = n; };

	float getVsp() { return vsp; };
	void setVsp(float n) { vsp = n; };

	float getImageIndex() { return imageIndex; };

	unsigned long getHp() { return hp; };
	void setHp(unsigned long n) { hp = n; if (hp > maxhp) { hp = maxhp; } };

	unsigned long getMaxHp() { return maxhp; };
	void setMaxHp(unsigned long n) { maxhp = n; };

	unsigned long getAmmo() { return ammo; };
	void setAmmo(unsigned long n) { ammo = n; if (ammo > maxammo) ammo = maxammo; }

	unsigned long getMaxAmmo() { return maxammo; };
	void setMaxAmmo(unsigned long n) { maxammo = n; };

	int getWeapon() { return weapon; };
	void setWeapon(int n);

	int getInvincibleTimer() { return invincibleTimer; };
	
	bool isPoisoned() { return poisonTimer > 0; };	

	bool isOnGround() { return onGround; };
	void setOnGround(bool on) { onGround = on; };

	void setCanDoubleJump(bool on) { canDoubleJump = on; };

private:
	void _hit(int damage, int x);

	HeroState state;

	float hsp, vsp;
	float imageIndex;

	unsigned long hp;
	unsigned long maxhp;

	unsigned long ammo;
	unsigned long maxammo;

	int weapon;
	int stoneState;

	int chargeTimer;
	int deathTimer;
	int drownTimer;
	int hitTimer;
	int invincibleTimer;
	int poisonTimer;
	int shieldTimer;
	int stoneTimer;
	int stunTimer;
	int quakeCounter;	

	bool heldUp;
	bool onGround;
	bool inWater;
	bool canCharge;
	bool canDoubleJump;
};