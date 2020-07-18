#include "gas.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../hero.hpp"

enum {
	STATE_WAIT,
	STATE_SMALL,
	STATE_LARGE,
	STATE_SMALL_AGAIN
};

Gas::Gas(float x, float y)
:Object(x,y)
{
	state = STATE_WAIT;
	timer = 0;
	imageIndex = 0;

	layer = 0;
}

void Gas::update()
{
	//animate
	if (state != STATE_WAIT)
		imageIndex += 0.2;

	switch (state)
	{
		case STATE_WAIT:
		{
			//wait for player to get near
			Rectangle area = (Rectangle){ (int)x - 50, (int)y - 10, 120, 30 };
			if (collision(area, hero.getMask()))
			{
				state = STATE_SMALL;
				imageIndex = 3;
				timer = 32;
				aud_PlaySound(sounds[sndGas01]);
			}
		}
		break;

		case STATE_SMALL:
		case STATE_SMALL_AGAIN:
		{
			if (imageIndex >= 5)
				imageIndex -= 2;

			if (timer-- <= 0)
			{
				if (state == STATE_SMALL_AGAIN)
					state = STATE_WAIT;
				else
				{
					state = STATE_LARGE;
					imageIndex = 0;
					timer = 175;
				}
			}

		}
		break;

		case STATE_LARGE:
		{
			imageIndex = fmod(imageIndex, 3);

			if (timer-- <= 0)
			{
				state = STATE_SMALL_AGAIN;
				timer = 120;
				imageIndex = 3;
			}

			//Poison hero
			if (!gameHasItem(ITEM_GAS_MASK))
			{
				Rectangle mask = (Rectangle){ (int)x+4, (int)y+6, 12, 12 };
				if (hero.hit(mask, 15))
					hero.poison();
			}
		}
		break;
	}
}

void Gas::draw(float subFrame, float depth)
{
	if (state == STATE_WAIT)
		return;

	gfx_DrawTile(images[imgEne01], x, y, 20, 20, 160 + (int)imageIndex);
}