#include "poisonbubble.hpp"
#include "../math.hpp"
#include "../assets.hpp"

PoisonBubble::PoisonBubble(float x, float y)
:Object(x,y)
{
	imageIndex = 0;

	layer = 0;
}

void PoisonBubble::update()
{
	Object::update();

	//movement
	y -= 1;

	//animate
	if ((imageIndex += 0.15) >= 6)
		dead = true;
}

void PoisonBubble::draw(float subFrame, float depth)
{
	float dy = lerp(y, prevy, subFrame);
	int animation[] = { 0, 1, 2, 1, 0, 3 };
	gfx_DrawTile(images[imgChr20], x, dy, 20, 20, 55 + animation[(int)imageIndex]);
}