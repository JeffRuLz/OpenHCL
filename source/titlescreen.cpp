#include "titlescreen.hpp"
#include "assets.hpp"
#include "input.hpp"
#include "text.hpp"
#include "system.hpp"
#include "game.hpp"
#include "math.hpp"
#include <string.h>

static int cursor = 0;
static int cursorSize = 0;
static int cursorX = 112;
static bool warning = false;
static bool load = false;

void titleScreenStart()
{
	aud_StopAll();

	cursor = 0;
	warning = false;
	load = false;

	cursorSize = 0;
	cursorSize = max(cursorSize, messageLength(TEXT_NEW_GAME));
	cursorSize = max(cursorSize, messageLength(TEXT_LOAD_GAME));
	cursorSize = max(cursorSize, messageLength(TEXT_EXIT));

	cursorSize = ceil(cursorSize * 8 / 16) + 2;

	cursorX = 160 - cursorSize * 16 / 2;
	//printf("%d %d\n", cursorSize, cursorX);

	if (sys_SaveFileExists(SYS_SAVEPATH))
		cursor = 1;
	
	if (sys_SaveFileExists(SYS_TEMPSAVEPATH))
	{
		cursor = 1;
		warning = true;
	}
}

void titleScreenUpdate()
{
	if (load == true)
	{
		loadAssets();

		//new game
		if (cursor == 0)
		{			
			gameResetGame();
			gameSetState(GAME_STATE_GAME);
		}

		//load game
		else if (cursor == 1)
		{
			//gameSetState(GAME_STATE_CREDITS);
			gameLoadGame();
			gameSetState(GAME_STATE_GAME);
		}
	}
	else
	{
		//move cursor
		int move = inp_ButtonPressed(BTN_DOWN) - inp_ButtonPressed(BTN_UP);

		if (move)
		{
			aud_PlaySound(sounds[sndPi01]);
			cursor += move;

			#ifndef _NO_EXIT
			 if (cursor < 0) cursor = 2;
			 if (cursor > 2) cursor = 0;
			#else
			 if (cursor < 0) cursor = 1;
			 if (cursor > 1) cursor = 0;
			#endif
		}

		//selection
		if (inp_ButtonPressed(BTN_ACCEPT) || inp_ButtonPressed(BTN_START))
		{
			//new game
			if (cursor == 0 || cursor == 1)
			{
				aud_PlaySound(sounds[sndOk]);
				load = true;
			}

			//exit
			else if (cursor == 2)
				sys_QuitGame();
		}
	}
}

void titleScreenDraw(float subFrame, float depth)
{
	if (load == true)
		return;

	if (!warning)
	{
		//Title image
		gfx_DrawSurfacePart(images[imgTitle01], 84, 36, 0, 0, 152, 84);
		//drawTextBold("HYDRA CASTLE LABYRINTH", 160, 28, TEXT_COLOR_YELLOW, true);
	}
	else
	{
		//Save error message
		drawText(TEXT_SAVE_ERROR + 0, 160, 40, true);
		drawText(TEXT_SAVE_ERROR + 1, 160, 65, true);
		drawText(TEXT_SAVE_ERROR + 2, 160, 88, true);
	}

	//cursor
	for (int i = 0; i < cursorSize; i++)
	{
		if (i == 0)
			gfx_DrawSurfacePart(images[imgTitle01], cursorX + i*16, 132 + (cursor*16), 0, 88, 16, 16);

		else if (i == cursorSize-1)
			gfx_DrawSurfacePart(images[imgTitle01], cursorX + i*16, 132 + (cursor*16), 80, 88, 16, 16);

		else
			gfx_DrawSurfacePart(images[imgTitle01], cursorX + i*16, 132 + (cursor*16), 16, 88, 16, 16);
	}

	drawTextBold(TEXT_NEW_GAME, 160, 136, TEXT_COLOR_YELLOW, true);
	drawTextBold(TEXT_LOAD_GAME, 160, 152, TEXT_COLOR_YELLOW, true);
	#ifndef _NO_EXIT
	drawTextBold(TEXT_EXIT, 160, 168, TEXT_COLOR_YELLOW, true);
	#endif

	drawBold("(C) 2011 E.HASHIMOTO", 160, 200, TEXT_COLOR_WHITE, true);
	drawBold("2020 OPEN HCL", 160, 210, TEXT_COLOR_WHITE, true);
}