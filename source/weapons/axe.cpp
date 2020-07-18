#include "axe.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"

#define grav 0.1

Axe::Axe(float x, float y, int direction)
:Object(x,y)
{
	hsp = 1 * sign(direction);
	vsp = -3;
	imageIndex = 0;
	mask = (Circle){ (int)x, (int)y, 9 };

	aud_PlaySound(sounds[sndShot01]);
	setDirection(hsp);
}

void Axe::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 8);

	//vertical movement
	y += vsp;
	vsp += grav;

	Rectangle r = (Rectangle){ (int)x-6, (int)y-6, 12, 12 };
	Rectangle collide;
	if (mapCollision(r, TILE_SOLID, &collide))
	{
		if (vsp < 0)
		{
			vsp = 0;
			y =  collide.y + 20 + 6;
		}
		else if (vsp > 0)
		{
			destroyEvent();
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
	}

	mask.x = x;
	mask.y = y;

	//outside of room
	if (x < 0 || x > 320 || y > 240)
		dead = true;
}

void Axe::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 96, 104 };

	gfx_DrawTile(images[imgChr20], dx - 10, dy - 10, 20, 20, tile[(hsp < 0)] + (int)imageIndex);
}

bool Axe::collide(Rectangle r, Object* obj)
{
	if (collision(r, mask))
	{
		destroyEvent();
		return true;
	}

	return false;
}

bool Axe::collide(Circle c, Object* obj)
{
	if (collision(c, mask))
	{
		destroyEvent();
		return true;
	}

	return false;
}

void Axe::destroyEvent()
{
	if (dead) return;
	
	aud_PlaySound(sounds[sndPi02]);
	effectList.add(new Tink(x,y));
	dead = true;
}