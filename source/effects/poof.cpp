#include "poof.hpp"
#include "../assets.hpp"

Poof::Poof(float x, float y)
:Object(x,y)
{
	imageIndex = 0;

	layer = 2;
}

void Poof::update()
{
	Object::update();

	if ((imageIndex += 0.3) >= 10)
		dead = true;
}

void Poof::draw(float subFrame, float depth)
{
	float dx = x + (-2 * depth);
	gfx_DrawTile(images[imgChr32], dx-16, y-6, 32, 32, (int)imageIndex);
}