#include "pebble.hpp"
#include "../assets.hpp"
#include "../math.hpp"

#define grav 0.05

Pebble::Pebble(float x, float y)
:Object(x,y)
{
	z = (rand() % 4) * -1;
	hsp = -0.5 + (0.125 * (rand() % 8));
	vsp = -1 - (0.125 * (rand() % 8));
	imageIndex = (rand() % 3);
	timer = 60;
	
	layer = 2;
}

void Pebble::update()
{
	Object::update();

	//movement
	x += hsp;
	y += (vsp += grav);

	if ((timer -= 1) <= 0)
		dead = true;
}

void Pebble::draw(float subFrame, float depth)
{
	if (timer < 30 && timer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	dx += z * depth;

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 24 + (int)imageIndex);
}