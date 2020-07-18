#include "shock.hpp"
#include "../assets.hpp"

Shock::Shock(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
	layer = 2;
}

void Shock::update()
{
	if ((imageIndex += 0.3) >= 6)
		dead = true;
}

void Shock::draw(float subFrame, float depth)
{
	float dx = x - 2 * depth;
	gfx_DrawTile(images[imgChr32], dx-16, y-16, 32, 32, 10 + (int)imageIndex);
}