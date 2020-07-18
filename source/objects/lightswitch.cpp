#include "lightswitch.hpp"
#include "input.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"

LightSwitch::LightSwitch(float x, float y, int flag, int keyFlag)
:Object(x,y)
{
	this->flag = flag;
	this->keyFlag = keyFlag;
	lit = false;
	imageIndex = 0;

	if (this->flag != 0)
		lit = gameGetFlag(this->flag);
}

void LightSwitch::update()
{
	if (lit)
	{
		imageIndex += 0.2;
		if (imageIndex >= 3)
			imageIndex -= 2;
	}
	else
	{
		if (inp_ButtonPressed(BTN_UP) && hero.isOnGround() && gameGetFlag(keyFlag))
		{
			Rectangle mask = (Rectangle){ (int)x, (int)y, 20, 20 };
			if (collision(hero.getX(), hero.getY(), mask))
			{
				aud_PlaySound(sounds[sndPi02]);
				lit = true;
				if (flag != 0)
					gameSetFlag(flag, true);
			}
		}
	}
}

void LightSwitch::draw(float subFrame, float depth)
{
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, 230 + (int)imageIndex);
}