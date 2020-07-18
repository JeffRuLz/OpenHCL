#include "arrow.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"

Arrow::Arrow(float x, float y, int direction)
:Object(x,y)
{
	hsp = 4 * direction;
	mask = (Rectangle){ (int)x-10, (int)y-1, 20, 2 };

	aud_PlaySound(sounds[sndShot01]);
	setDirection(hsp);
}

void Arrow::update()
{
	Object::update();

	x += hsp;

	mask = (Rectangle){ (int)x-10, (int)y-1, 20, 2 };

	if (mapCollision(mask, TILE_SOLID))
		destroyEvent();

	//destroy if outside of room
	if (x < -20 || x > 340)
		dead = true;
}

void Arrow::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, 86 + (hsp < 0));
}

bool Arrow::collide(Rectangle r, Object* obj)
{
	if (collision(r, mask))
	{
		destroyEvent();
		return true;
	}

	return false;
}

bool Arrow::collide(Circle c, Object* obj)
{
	if (collision(c, mask))
	{
		destroyEvent();
		return true;
	}

	return false;
}

void Arrow::destroyEvent()
{
	if (dead) return;

	aud_PlaySound(sounds[sndPi02]);
	effectList.add(new Tink(x,y));
	dead = true;
}