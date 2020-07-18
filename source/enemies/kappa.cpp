#include "kappa.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/splash.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

#define grav (0.0625)

enum {
	STATE_WAIT,
	STATE_JUMP,
	STATE_WALK
};

enum {
	TYPE_WATER,
	TYPE_LAND,
	TYPE_LAVA
};

Kappa::Kappa(float x, float y, int subtype, int jumpHeight, int jumpDelay, int startDelay)
:Object(x,y+8)
{
	hp = 2;

	state = STATE_WAIT;
	type = subtype;
	ang = 0;
	timer = startDelay;
	delay = jumpDelay;
	ystart = this->y;
	imageIndex = 0;
	hsp = 0;
	vsp = 0;
	this->jumpHeight = jumpHeight / 4.f;
	jumpDown = false;
}

void Kappa::update()
{
	Object::update();

	switch (state)
	{
		case STATE_WAIT:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.1, 2);

			//bob
			ang = (ang + 5) % 360;
			y = ystart + lengthdir_y(2.5, ang);

			//jump
			if ((timer -= 1) <= 0)
			{
				state = STATE_JUMP;
				imageIndex = 0;
				y = ystart;
				vsp = -jumpHeight;

				if (type == TYPE_LAVA)
				{
					aud_PlaySound(sounds[sndShot07]);
					createSplash(x+10, y+10, true);
				}
				else
				{
					aud_PlaySound(sounds[sndWater01]);
					createSplash(x+10, y+10, false);
				}

				if (type == TYPE_WATER)
				{
					if (x + 10 < hero.getX())
						hsp = 1;
					else
						hsp = -1;
				}
			}
		}
		break;

		case STATE_JUMP:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.25, 3);

			//movement
			y += (vsp += grav);
			x += hsp;

			if (vsp > 0)
			{
				if (type == TYPE_LAND && !jumpDown)
				{
					Rectangle collide;
					if (!solidCollision(x+10,y+10) && solidCollision(x+10,y+20,&collide))
					{
						y = collide.y - 20;
						state = STATE_WALK;
						imageIndex = 0;
						timer = 240;
						hsp = ((hero.getX() > x+10)? 1: -1);
					}
				}

				//land in water
				if (y >= ystart)
				{
					jumpDown = false;
					y = ystart;
					state = STATE_WAIT;
					vsp = 0;
					timer = delay;

					if (type == TYPE_LAVA)
					{
						aud_PlaySound(sounds[sndShot07]);
						createSplash(x+10, y+10, true);
					}
					else
					{
						aud_PlaySound(sounds[sndWater01]);
						createSplash(x+10, y+10, false);
					}
				}
			}
		}
		break;

		case STATE_WALK:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.15, 2);

			//movement
			x += hsp;

			//hit wall / turn on ledge
			if (solidCollision(x+10+10*sign(hsp), y+10) || !solidCollision(x+10+10*sign(hsp), y+30))
				hsp *= -1;

			if ((timer -= 1) <= 0)
			{
				aud_PlaySound(sounds[sndPi02]);
				state = STATE_JUMP;
				jumpDown = true;
				hsp = 0;
				vsp = -2;
				imageIndex = 0;
				timer = 60;
			}
		}
		break;
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+2, 16, 16 };

	hero.hit(mask, 15);

	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
			aud_PlaySound(sounds[sndHit02]);
		else
		{
			aud_PlaySound(sounds[sndBom01]);
			effectList.add(new Poof(x+10,y));
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Kappa::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 176, 181, 215 };
	int t = tile[type] + (int)imageIndex;

	if (state == STATE_JUMP)
		t += 2;
	else if (state == STATE_WALK)
		t += 5 + ((hsp < 0)? 2: 0);

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, t);
}