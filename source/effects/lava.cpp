#include "lava.hpp"
#include "../assets.hpp"
#include "../math.hpp"

enum {
	TYPE_LAVA,
	TYPE_WATER
};

Lava::Lava(float x, float y, int type)
:Object(x,y)
{
	this->type = type;
	imageIndex = 0;
	
	layer = 0;
}

void Lava::update()
{
	if (type == TYPE_LAVA)
		imageIndex = fmod(imageIndex + 0.125, 3);

	else if (type == TYPE_WATER)
		imageIndex = fmod(imageIndex + 0.125, 4);
}

void Lava::draw(float subFrame, float depth)
{
	int tile[] = { 18, 22 };
	gfx_DrawTile(images[imgTiles], x, y, 20, 20, tile[type] + (int)imageIndex);
}