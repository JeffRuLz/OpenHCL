#include "airbubble.hpp"
#include "../assets.hpp"
#include "../math.hpp"

AirBubble::AirBubble(float x, float y)
:Object(x,y)
{
	z = rand() % 5;
	xstart = this->x;
	imageIndex = 0;
	timer = 120;

	layer = 0;
}

void AirBubble::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.2, 2);

	//movement
	if (mapCollision(x,y,TILE_WATER))
		y -= 0.25;

	x = xstart + lengthdir_x(2.5, timer*5);

	if ((timer -= 1) <= 0)
		dead = true;
}

void AirBubble::draw(float subFrame, float depth)
{
	if (timer <= 30 && timer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgChr20], dx-10+z*depth, dy-10, 20, 20, 11 + (int)imageIndex);
}