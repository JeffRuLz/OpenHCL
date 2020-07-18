#include "platform.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/brokenrock.hpp"

enum {
	TYPE_HORIZONTAL,
	TYPE_VERTICAL,
	TYPE_BREAKABLE,
	TYPE_DISAPPEAR,
	TYPE_STILL
};

static bool playSound = true;

Platform::Platform(float x, float y, int type, int arg1, int arg2, int arg3)
:Object(x,y)
{
	this->type = type;
	speed = arg1 / 2.f;
	distance = arg2;
	direction = 1;
	timer = arg2;
	breakTimer = 30;
	shake = false;
	visible = true;
	invisibleTime = 0;
	visibleTime = 0;
	xstart = this->x;

	if (this->type == TYPE_BREAKABLE)
	{
		speed = 0;
		distance = 1;
		timer = arg2;
	}

	if (this->type == TYPE_DISAPPEAR)
	{
		speed = 0;
		distance = 0;
		invisibleTime = arg1;
		visibleTime = arg2;
		timer = arg3;
		visible = false;

		if (timer == 0)
		{
			visible = true;
			timer = visibleTime;
		}
	}
}

void Platform::update()
{
	Object::update();

	Rectangle mask = (Rectangle){ (int)x, (int)y, 20, 20 };

	switch (type)
	{
		case TYPE_BREAKABLE:
		case TYPE_HORIZONTAL:
		{
			bool moveHero = false;

			x += speed * direction;

			if (hero.isOnGround())
			{
				mask.x = (int)x;
				mask.y -= 1;
				if (collision(hero.getMask(), mask))
				{
					moveHero = true;
					if (type == TYPE_BREAKABLE && shake == false)
					{
						shake = true;
						speed = 1;
					}
				}
					
			}

			if (moveHero)
			{
				hero.setX(hero.getX() + speed * direction);
			}

			if ((timer -= 1) <= 0)
			{
				direction *= -1;
				timer = distance;
			}

			if (shake)
			{
				if ((breakTimer -= 1) <= 0)
				{
					aud_PlaySound(sounds[sndBom02]);
					rockSmashEffect(x+10, y+10);
					dead = true;
				}
			}
		}
		break;

		case TYPE_VERTICAL:
		{
			bool heroCheck = false;
			bool heroCollide = collision(mask, hero.getMask());

			if (hero.isOnGround())
			{
				mask.y -= 1;

				if (hero.getY() < y && collision(mask, hero.getMask()))
					heroCheck = true;

				mask.y = y;
			}

			y += speed * direction;

			if (heroCheck)
				hero.setY(y - 10);
			else
			{
				mask.y = y;

				if (direction == -1 && heroCollide == false && hero.getY() < y && collision(mask, hero.getMask()))
					hero.setY(y - 10);
			}

			if ((timer -= 1) <= 0)
			{
				direction *= -1;
				timer = distance;
			}
		}
		break;

		case TYPE_DISAPPEAR:
		{
			if (timer % 5 == 0)
				playSound = true;
			
			if (timer > 0)
				timer -= 1;

			if (visible)
			{
				warpX(xstart);

				if (timer <= 0)
				{
					visible = false;
					timer = invisibleTime;
					playSound = true;
				}
			}
			else
			{
				warpX(1280);

				if (timer <= 0)
				{
					if (playSound)
					{
						aud_PlaySound(sounds[sndPi03]);
						playSound = false;
					}

					visible = true;
					timer = visibleTime;
				}
			}
		}
		break;
	}
}

void Platform::draw(float subFrame, float depth)
{
	if (!visible)
		return;

	if (type == TYPE_DISAPPEAR && timer <= 30 && timer % 2 == 0)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 160, 160, 161, 162, 169 };

	gfx_DrawTile(images[imgChr20], dx, dy, 20, 20, tile[type]);
}