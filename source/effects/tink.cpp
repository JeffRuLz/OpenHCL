#include "tink.hpp"
#include "../assets.hpp"

Tink::Tink(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
	layer = 2;
}

void Tink::update()
{
	Object::update();

	if ((imageIndex += 0.25) >= 5)
		dead = true;
}

void Tink::draw(float subFrame, float depth)
{
	float dx = x + (-3 * depth);
	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, 27 + (int)imageIndex);
}