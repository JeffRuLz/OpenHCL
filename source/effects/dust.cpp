#include "dust.hpp"
#include "../assets.hpp"
#include "../math.hpp"

Dust::Dust(float x, float y, int direction)
:Object(x,y)
{
	imageIndex = 0;
	setDirection(direction);

	layer = 2;
}

void Dust::update()
{
	Object::update();

	x += 0.5 * direction;

	//animate
	if ((imageIndex += 0.3) >= 8)
		dead = true;
}

void Dust::draw(float subFrame, float depth)
{
	int tile = 32 + (8 * (direction == -1));
	float dx = lerp(x, prevx, subFrame);
	dx += -2 * depth;

	gfx_DrawTile(images[imgChr20], dx, y, 20, 20, tile + (int)imageIndex);
}