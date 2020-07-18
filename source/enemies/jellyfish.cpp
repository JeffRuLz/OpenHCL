#include "jellyfish.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_WAIT,
	STATE_ATTACK,
	STATE_STABILIZE
};

Jellyfish::Jellyfish(float x, float y, int bobDelay)
:Object(x,y)
{
	state = STATE_WAIT;
	ang = bobDelay * 5;
	ystart = this->y;
	speed = 0;
	imageIndex = 0;
}

void Jellyfish::update()
{
	Object::update();

	if (state == STATE_ATTACK)
	{
		//movement
		x += lengthdir_x(speed, ang);
		y += lengthdir_y(speed, ang);

		//slow down
		if ((speed -= 0.0375) <= 0)
		{
			state = STATE_STABILIZE;
			imageIndex = 0;
			speed = 0.5;
			ystart = y-10;
			ang = 90;
		}
	}
	else
	{
		//animate
		imageIndex = fmod(imageIndex + 0.05, 4);

		//floating motion
		ang = fmod(ang + 2.5, 360);
		y = ystart + lengthdir_y(10, ang);

		if (state == STATE_WAIT)
		{
			//check if player is near
			Rectangle area = (Rectangle){ (int)x-30, (int)y-30, 80, 80 };
			if (collision(hero.getX(), hero.getY(), area))
			{
				state = STATE_ATTACK;
				aud_PlaySound(sounds[sndPi02]);
				speed = 1.5;

				if (hero.getY() > y+10) //down
				{
					if (hero.getX() < x+10) //left
					{
						ang = 135;
						imageIndex = 5;
					}
					else //right
					{
						ang = 45;
						imageIndex = 6;
					}
				}
				else //up
				{
					if (hero.getX() < x+10) //left
					{
						ang = 225;
						imageIndex = 4;
					}
					else //right
					{
						ang = 315;
						imageIndex = 3;
					}
				}
			}
		}
		else if (state == STATE_STABILIZE)
		{
			if (ang >= 180)
			{
				state = STATE_WAIT;
				ystart = y-10;
				ang = 90;
			}
		}
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+3, (int)y+3, 14, 14 };

	hero.hit(mask, 15);

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Jellyfish::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	dx += (-2 - lengthdir_x(2, ang)) * depth;

	int tile = (int)imageIndex;

	if (state != STATE_ATTACK)
	{
		int animation[] = { 0, 1, 0, 2 };
		tile = animation[(int)imageIndex];
	}

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, 208 + tile);
}