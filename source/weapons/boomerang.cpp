#include "boomerang.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/sparkle.hpp"
#include "../effects/tink.hpp"

Boomerang::Boomerang(float x, float y, int direction)
:Object(x,y)
{
	hsp = 3 * direction;
	imageIndex = 0;
	timer = 120;
	dir = direction;
	mask = (Circle){ (int)x, (int)y, 9 };

	aud_PlaySound(sounds[sndShot01]);
	setDirection(hsp);
}

void Boomerang::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 8);

	//horizontal movement
	x += hsp;
	hsp -= 0.0625 * dir;
	setDirection(hsp);

	//limit speed
	if (abs(hsp) > 3)
		hsp = 3 * sign(hsp);

	//vertical movement
	y = hero.getY();

	mask.x = x;
	mask.y = y;

	//kill
	if (timer-- <= 0)
	{
		effectList.add(new Sparkle(x,y));
		dead = true;
	}
}

void Boomerang::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 72 + (int)imageIndex;

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, tile);
}

static void _addEffect(float x1, float y1, float x2, float y2)
{
	float x = ((x1 + x2) / 2) - 10 + (rand() % 20);
	float y = ((y1 + y2) / 2) - 10 + (rand() % 20);
	effectList.add(new Tink(x,y));
}

bool Boomerang::collide(Rectangle r, Object* obj)
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

bool Boomerang::collide(Circle c, Object* obj)
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