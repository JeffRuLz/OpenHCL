#include "skeleton.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"
#include "../effects/brokenrock.hpp"
#include "../objects/collectable.hpp"

#define walkSpd 0.25
#define fric 0.0375

enum {
	STATE_WALK,
	STATE_ALERT,
	STATE_SLIDE,
	STATE_THROW
};

Skeleton::Skeleton(float x, float y, bool flip)
:Object(x,y)
{
	hp = 2;
	state = STATE_WALK;
	direction = ((flip == false)? 1: -1);
	timer = 0;
	hsp = walkSpd * direction;
	imageIndex = 0;
}

void Skeleton::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	//animate
	if (state != STATE_SLIDE)
		imageIndex = fmod(imageIndex + 0.1, 2);

	//movement
	if (hsp != 0)
	{
		x += hsp;

		//collide with wall / turn on ledge
		if (solidCollision(x+10+8*sign(hsp), y) || !solidCollision(x+10+8*sign(hsp), y+30))
		{
			hsp *= -1;
		}
	}

	switch (state)
	{
		case STATE_WALK:
		{
			direction = ((hsp > 0)? 1: -1);

			//check if hero is near
			if (timer <= 0)
			{
				Rectangle area = (Rectangle){ (int)x-40, (int)y-10, 100, 40 };
				if (collision(hero.getMask(), area))
				{
					state = STATE_ALERT;
					timer = 30;
					hsp = 0;
					direction = ((hero.getX() > x+10)? 1: -1);
				}
			}
		}
		break;

		case STATE_ALERT:
		{
			hsp = 0;

			if (timer <= 0)
			{
				state = STATE_SLIDE;
				hsp = 1.25 * -direction;
				aud_PlaySound(sounds[sndPi05]);
			}
		}
		break;

		case STATE_SLIDE:
		{
			imageIndex = 0;

			if (hsp > 0)
			{
				if ((hsp -= fric) <= 0)
					hsp = 0;
			}
			else if (hsp < 0)
			{
				if ((hsp += fric) >= 0)
					hsp = 0;
			}

			if (hsp == 0)
			{
				state = STATE_THROW;
				timer = 30;
				enemyList.add(new Bone(x+10,y+10,direction));
				aud_PlaySound(sounds[sndShot05]);
			}
		}
		break;

		case STATE_THROW:
		{
			if (timer <= 0)
			{
				timer = 0;
				state = STATE_WALK;
				hsp = walkSpd * direction;
			}
		}
		break;
	}

	Rectangle mask = (Rectangle){ (int)x+4, (int)y+2, 12, 18 };

	hero.hit(mask, 10);

	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
			aud_PlaySound(sounds[sndHit02]);
		else
		{
			aud_PlaySound(sounds[sndBom02]);
			rockSmashEffect(x+10, y+10);
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Skeleton::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	int tile = 100 + ((direction == -1)? 2: 0);
	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, tile + (int)imageIndex);
}


Bone::Bone(float x, float y, int direction)
:Object(x,y)
{
	hsp = direction * 0.375;
	vsp = -2;
	imageIndex = 0;
}

void Bone::update()
{
	Object::update();

	imageIndex += 0.25 * sign(hsp);
	if (imageIndex >= 8) imageIndex -= 8;
	if (imageIndex < 0) imageIndex += 8;

	x += hsp;
	y += (vsp += 0.05);

	Circle mask = (Circle){ (int)x, (int)y, 6 };

	if (collision(hero.getShieldMask(), mask))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	else
	{
		hero.hit(mask, 10);
	}

	if (y > 250)
		dead = true;
}

void Bone::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgEne01], dx-10, dy-10, 20, 20, 104 + (int)imageIndex);
}