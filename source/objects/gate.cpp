#include "gate.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"

Gate::Gate(float x, float y, int color, int keyFlag)
:Object(x,y)
{
	opened = false;
	red = color;
	this->keyFlag = keyFlag;
	imageIndex = 0;
}

void Gate::update()
{
	//opened
	if (imageIndex >= 4)
		return;

	if (hitTimer > 0)
		hitTimer -= 1;

	Rectangle mask = (Rectangle){ (int)x+5, (int)y, 10, 20 };

	//hero collision
	Rectangle hMask = hero.getMask();
	if (collision(mask, hMask))
	{
		if (hero.getX() > x+10)
			hero.setX(x+10 + mask.w/2 + hMask.w/2);
		else
			hero.setX(x+10 - mask.w/2 - hMask.w/2);
	}

	if (opened)
	{
		imageIndex += 0.1;
	}
	else
	{
		if (weaponList.collide(mask, this))
		{
			hitTimer = 15;
			
			if (!gameGetFlag(keyFlag))
			{
				aud_PlaySound(sounds[sndHit03]);
			}
			else
			{
				opened = true;
				aud_PlaySound(sounds[sndDoor00]);	
			}	
		}
	}
}

void Gate::draw(float subFrame, float depth)
{
	int tile[] = { 208, 213 };
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, tile[red] + (int)imageIndex);
}