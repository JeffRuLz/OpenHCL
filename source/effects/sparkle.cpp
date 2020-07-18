#include "sparkle.hpp"
#include "../assets.hpp"

Sparkle::Sparkle(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
	layer = 1;
}

void Sparkle::update()
{
	Object::update();

	if ((imageIndex += 0.3) >= 5)
		dead = true;
}

void Sparkle::draw(float subFrame, float depth)
{
	float dx = x + (-4 * depth);
	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, 59 + (int)imageIndex);
}