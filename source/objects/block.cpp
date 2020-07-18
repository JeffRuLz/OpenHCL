#include "block.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "collectable.hpp"
#include "../effects/brokenrock.hpp"

Block::Block(float x, float y, int subtype, int tile)
:Object(x,y)
{
	hp = 3;
	this->tile = tile;
	type = subtype;

	solid = true;

	if (type != 1 && this->tile == 0)
	{
		int id = ((int)x/20 + 3) + ((int)y/20 + 3) * 22;
		this->tile = map[1].data[id];
		map[1].data[id] = 0;
	}
}

void Block::create()
{
	if (gameGetFlag(killFlag) || gameGetFlag(despawnFlag))
		dead = true;
}

void Block::update()
{
	Object::update();

	Rectangle mask = (Rectangle){ (int)x, (int)y, 20, 20 };
	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;
		
		if (type == 1 || (!gameHasItem(ITEM_COPPER_PICK) && !gameHasItem(ITEM_SILVER_PICK)))
			aud_PlaySound(sounds[sndHit03]);
		else
		{
			hp -= (gameHasItem(ITEM_SILVER_PICK))? hp: 1;

			if (hp > 0)
				aud_PlaySound(sounds[sndHit02]);
			
			else
			{
				aud_PlaySound(sounds[sndBom02]);
				rockSmashEffect(x+10, y+10);
				//don't drop item if it's hiding something
				if (killFlag <= 0)
					dropCollectable(x,y);

				dead = true;
			}		
		}	
	}
}

void Block::draw(float subFrame, float depth)
{
	if (type == 1)
		gfx_DrawTile(images[imgChr20], x, y, 20, 20, 163);
	else
		gfx_DrawTile(images[imgTiles], x, y, 20, 20, tile);
}