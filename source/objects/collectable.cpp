#include "collectable.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/splash.hpp"

#define grav 0.1f

enum {
	TYPE_HEART,
	TYPE_AMMO
};

void dropCollectable(float x, float y)
{
	int hasGem = gameHasItem(ITEM_GOLD_GEM);
	bool lucky = (rand() % 100 < 50);

	if (rand() % 100 < (20 * (hasGem + 1)))
	{
		int type = TYPE_HEART;

		if (rand() % 100 < 50)
		{
			type = TYPE_AMMO;
			if (hasGem && lucky && hero.getAmmo() >= hero.getMaxAmmo())
				type = TYPE_HEART;
		}
		else
		{
			type = TYPE_HEART;
			if (hasGem && lucky && hero.getHp() >= hero.getMaxHp())
				type = TYPE_AMMO;
		}

		objectList.add(new Collectable(x,y,type));
	}
}

Collectable::Collectable(float x, float y, char type)
:Object(x,y)
{
	this->type = type;
	counter = 0;
	timer = 30;
	vsp = -1.25;
	firstFrame = true;
}

void Collectable::update()
{
	//check if it spawned inside of a wall
	if (firstFrame)
	{
		if (solidCollision(x+10, y+10))
		{
			//pop upwards if there is a free spot
			Rectangle collide;
			if (solidCollision(x+10, y+30, &collide) && !solidCollision(x+10, y-10))
			{
				y = collide.y - 20;
			}
		}

		firstFrame = false;
	}

	Object::update();

	//animate
	if (timer > 0)
		timer -= 1;

	//movement
	y += (vsp += grav);

	//fall
	if (vsp > 0)
	{
		Rectangle collide;
		if (!solidCollision(x+10,y) && solidCollision(x+10,y+20,&collide))
		{
			y = collide.y - 20;

			//bounce
			if (counter > 2)
				vsp = 0;
			else
			{
				float bounceVsp[] = { -1, -0.5, 0 };

				vsp = bounceVsp[counter];
				aud_PlaySound(sounds[sndPi02]);
				counter += 1;
			}
		}
	}

	//collision
	Rectangle mask = (Rectangle){ (int)x+3, (int)y+7, 14, 13 };

	if (collision(mask, hero.getMask()))
	{
		aud_PlaySound(sounds[sndGet02]);

		if (type == TYPE_HEART)
			hero.setHp(hero.getHp() + 10);

		else if (type == TYPE_AMMO)
			hero.setAmmo(hero.getAmmo() + 5);

		dead = true;
	}

	if (mapCollision(x+10,y+10,TILE_LAVA))
	{
		createSplash(x+10, y+10, true);
		aud_PlaySound(sounds[sndPi02]);
		dead = true;
	}

	//outside of room
	if (y > 240)
		dead = true;
}

void Collectable::draw(float subFrame, float depth)
{
	if (timer % 2)
		return;

	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgChr20], x, dy, 20, 20, 48 + type);
}