#include "bossmusiccontroller.hpp"
#include "../assets.hpp"
#include "../game.hpp"

BossMusicController::BossMusicController(int flag)
:Object(0,0)
{
	this->flag = flag;
	check = false;
}

void BossMusicController::update()
{
	if (!check)
	{
		check = true;

		if (flag != 0 && !gameGetFlag(flag))
		{
			gameSetBossRoom(true);
			aud_StopMusic();
			aud_PlayMusic(music[bgmBoss], true);

			if (gameHasItem(ITEM_BELL))
				aud_PlaySound(sounds[sndBell01]);
		}
		else
		{
			dead = true;
			return;
		}
	}

	if (flag == 0 || gameGetFlag(flag))
	{
		gamePlaySecret();
		killFlag = 0;
		dead = true;
	}
}