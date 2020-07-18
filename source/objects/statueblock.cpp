#include "statueblock.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/brokenrock.hpp"

StatueBlock::StatueBlock(float x, float y, int type, int keyFlag)
:Object(x,y)
{
	hp = 3;
	this->type = type;
	this->keyFlag = keyFlag;
	blinkTimer = 0;
}

void StatueBlock::update()
{
	Object::update();

	if (blinkTimer > 0)
		blinkTimer -= 1;

	Rectangle mask = (Rectangle){ (int)x, (int)y, 20, 20 };
	Rectangle hMask = hero.getMask();

	//stop hero
	if (collision(mask, hMask))
	{
		if (hero.getX() > x+10)
			hero.setX(mask.x+mask.w + hMask.w/2);
		else
			hero.setX(mask.x - hMask.w/2);
	}

	//
	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;

		if (!gameGetFlag(keyFlag))
		{
			aud_PlaySound(sounds[sndHit03]);
		}
		else
		{
			hp -= 1;
			blinkTimer = 15;			

			if (hp > 0)
				aud_PlaySound(sounds[sndHit02]);
			else
			{
				aud_PlaySound(sounds[sndBom02]);
				rockSmashEffect(x+10, y+10);
				dead = true;
			}
		}
	}
}

void StatueBlock::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;
	
	int tile[] = { 165, 0, 0, 0, 170 };
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, tile[type]);
}