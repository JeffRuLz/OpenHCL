#include "savepoint.hpp"
#include "input.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"

SavePoint::SavePoint(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
}

void SavePoint::update()
{
	//animate
	imageIndex += 0.15;
	if (imageIndex >= 4)
		imageIndex -= 4;

	if (inp_ButtonPressed(BTN_UP) && hero.isOnGround() && gameGetState() == GAME_STATE_GAME)
	{
		Rectangle mask = (Rectangle){ (int)x+3, (int)y, 14, 20 };
		if (collision(hero.getX(), hero.getY(), mask))
		{
			aud_PlaySound(sounds[sndPower02]);
			hero.setHp(hero.getMaxHp());
			gameSetState(GAME_STATE_SAVE);
		}
	}
}

void SavePoint::draw(float subFrame, float depth)
{
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, 128 + (int)imageIndex);
}