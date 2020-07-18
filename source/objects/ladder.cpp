#include "ladder.hpp"
#include "../assets.hpp"
#include "../map.hpp"
#include "../game.hpp"

Ladder::Ladder(float x, float y, int height)
:Object(x,y)
{
	this->height = height;
	set = false;
}

void Ladder::create()
{
	if (gameGetLevel() == 7 && spawnFlag != 0 && !gameGetFlag(spawnFlag))
		gameRingBell();
}

void Ladder::update()
{
	if (!set)
	{
		int mx = x / 20 + 3;
		int my = y / 20 + 3;

		for (int i = 0; i < height; i++)			
			map[1].data[mx + (my+i) * 22] = 3;

		set = true;
	}
}

void Ladder::draw(float subFrame, float depth)
{
	for (int i = 0; i < height; i++)
		gfx_DrawTile(images[imgTiles], x, y+i*20, 20, 20, 3);
}