#include "bomb.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"

enum {
	STATE_NORMAL,
	STATE_EXPLOSION
};

#define grav 0.05

Bomb::Bomb(float x, float y, int direction)
:Object(x,y)
{
	state = STATE_NORMAL;
	hsp = 0.5 * direction;
	vsp = -1;
	imageIndex = 0;
	counter = 0;
	mask = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };

	aud_PlaySound(sounds[sndShot01]);
	setDirection(hsp);
}

void Bomb::update()
{
	Object::update();

	switch (state)
	{
		case STATE_NORMAL:
		{
			//animate
			imageIndex -= 0.3 * sign(hsp);

			if (imageIndex < 0) imageIndex += 8;
			if (imageIndex >= 8) imageIndex -= 8;

			//vertical movement
			y += (vsp += grav);

			mask = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };
			Rectangle collide;
			if (solidCollision(mask, &collide))
			{
				if (vsp < 0)
					y = collide.y + 20 + 6;
				else if (vsp > 0)
				{
					y = collide.y - 6;

					if (counter == 0)
					{
						vsp = -1;
						aud_PlaySound(sounds[sndPi02]);
					}
					else if (counter == 1)
					{
						vsp = -0.5;
						aud_PlaySound(sounds[sndPi02]);
					}
					else
					{
						state = STATE_EXPLOSION;
						imageIndex = 0;
						aud_PlaySound(sounds[sndBom03]);
					}

					counter += 1;
				}
			}

			//horizontal movement
			x += hsp;

			mask = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };
			if (solidCollision(mask, &collide))
			{
				if (hsp > 0)
					x = collide.x - 6;
				else if (hsp < 0)
					x = collide.x + 20 + 6;

				hsp *= -1;
				setDirection(hsp);
			}
		}
		break;

		case STATE_EXPLOSION:
		{
			mask = (Rectangle){ (int)x-32, (int)y-32-4, 64, 55 };

			hero.hit(mask, 20);

			//animate
			imageIndex += 0.3;

			if (imageIndex >= 11)
				dead = true;
		}
		break;
	}

	//outside of room
	if (x < -10 || x > 330 || y > 250)
		dead = true;
}

void Bomb::draw(float subFrame, float depth)
{
	switch (state)
	{
		case STATE_NORMAL:
		{
			float dx = lerp(x, prevx, subFrame);
			float dy = lerp(y, prevy, subFrame);

			int tile = 64 + (int)imageIndex;

			gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, tile);
		}
		break;

		case STATE_EXPLOSION:
		{
			gfx_DrawTile(images[imgChr64], x-32, y-36, 64, 48, (int)imageIndex);
		}
		break;
	}
}

bool Bomb::collide(Rectangle r, Object* obj)
{
	if (state == STATE_NORMAL)
		return false;

	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}

	return collision(r, mask);
}

bool Bomb::collide(Circle c, Object* obj)
{
	if (state == STATE_NORMAL)
		return false;

	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}

	return collision(c, mask);
}