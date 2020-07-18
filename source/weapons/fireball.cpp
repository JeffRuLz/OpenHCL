#include "fireball.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"

#define grav 0.05

Fireball::Fireball(float x, float y, int direction)
:Object(x,y)
{
	hsp = 1 * direction;
	vsp = -1;
	imageIndex = 0;
	counter = 0;
	mask = (Circle){ (int)x, (int)y, 10 };

	aud_PlaySound(sounds[sndShot01]);
	setDirection(hsp);
}

void Fireball::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.5, 8);

	//vertical movement
	y += (vsp += grav);

	Rectangle r = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };
	Rectangle collide;
	if (mapCollision(r, TILE_SOLID, &collide))
	{
		if (vsp < 0)
		{
			y = collide.y + 20 + 6;
		}
		else if (vsp > 0)
		{
			y = collide.y - 6;
			
			if (counter == 0)
				vsp = -1;
			else if (counter == 1)
				vsp = -0.5;
			else
			{
				effectList.add(new Tink(x,y));
				dead = true;
			}

			counter += 1;
			aud_PlaySound(sounds[sndPi02]);
		}
	}

	//horizontal movement
	x += hsp;

	r = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };
	if (mapCollision(r, TILE_SOLID, &collide))
	{
		if (hsp > 0)
			x = collide.x - 6;
		else if (hsp < 0)
			x = collide.x + 20 + 6;

		hsp *= -1;
		setDirection(hsp);
	}

	mask.x = x;
	mask.y = y;

	//outside of room
	if (x < -20 || x > 340 || y > 320)
		dead = true;
}

void Fireball::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 112, 120 };

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, tile[(hsp < 0)] + (int)imageIndex);
}

static void _addEffect(float x1, float y1, float x2, float y2)
{
	float x = ((x1 + x2) / 2) - 10 + (rand() % 20);
	float y = ((y1 + y2) / 2) - 10 + (rand() % 20);
	effectList.add(new Tink(x,y));
}

bool Fireball::collide(Rectangle r, Object* obj)
{
	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}

	if (collision(r, mask))
	{
		_addEffect(r.x+r.w/2, r.y+r.h/2, mask.x, mask.y);
		return true;
	}

	return false;
}

bool Fireball::collide(Circle c, Object* obj)
{
	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}
	
	if (collision(c, mask))
	{
		_addEffect(c.x, c.y, mask.x, mask.y);
		return true;
	}

	return false;
}