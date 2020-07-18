#include "door.hpp"
#include "input.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"

Door::Door(float x, float y, int keyFlag, int openFlag, int level, int room, int warpx, int warpy)
:Object(x,y)
{
	this->keyFlag = keyFlag;
	this->openFlag = openFlag;
	this->level = level;
	this->room = room;
	this->warpx = warpx * 20 + 10;
	this->warpy = warpy * 20 + 10;
	opened = (gameGetFlag(this->openFlag) || this->level == 0);
}

void Door::update()
{
	if (inp_ButtonPressed(BTN_UP) && hero.isOnGround())
	{
		Rectangle mask = (Rectangle){ (int)x+3, (int)y, 14, 20 };

		if (collision(hero.getX(), hero.getY(), mask))
		{
			if (opened == false)
			{
				if (gameGetFlag(keyFlag))
				{
					aud_PlaySound(sounds[sndDoor00]);
					gameSetFlag(openFlag, true);
					opened = true;
				}
				else
				{
					gameDoorMessage(level);
				}
			}
			else
			{
				hero.setX(x+10);
				hero.setY(y+10);
				hero.setState(HERO_STATE_DOOR);
				aud_PlaySound(sounds[sndStep01]);

				gameSetWarp(warpx, warpy, level, room);
			}			
		}
	}
}

void Door::draw(float subFrame, float depth)
{
	float dx = x + 2 * depth;
	gfx_DrawTile(images[imgChr20], dx, y, 20, 20, 14 + !opened);
}