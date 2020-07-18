#include "floorbutton.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"

FloorButton::FloorButton(float x, float y, int flag)
:Object(x,y)
{
	this->flag = flag;
	pressed = false;
}

void FloorButton::update()
{
	if (pressed == false && hero.isOnGround())
	{
		Rectangle mask = (Rectangle) { (int)x+6, (int)y + 15, 8, 5 };
		if (collision(mask, hero.getMask()))
		{
			aud_PlaySound(sounds[sndHit02]);
			pressed = true;

			if (flag != 0)
				gameSetFlag(flag, true);
		}
	}
}

void FloorButton::draw(float subFrame, float depth)
{
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, 132 + pressed);
}