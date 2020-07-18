#include "options.hpp"
#include "assets.hpp"
#include "input.hpp"
#include "text.hpp"
#include "game.hpp"
#include "math.hpp"
#include "ini.hpp"
#include "system.hpp"

static char page = 0;
static int cursor = 0;

enum {
	OPTION_LANGUAGE = 0,

   #ifdef _CAN_RESIZE
	OPTION_RESIZE,
   #endif

   #ifdef _HAS_FULLSCREEN
	OPTION_FULLSCREEN,
   #endif

   #ifdef _HAS_VIDEOMODE
	OPTION_VIDEOMODE,
   #endif
   
   #ifdef _HAS_VSYNC
	OPTION_VSYNC,
   #endif

	OPTION_MUSIC,
	OPTION_SOUND,
	OPTION_TIMER,	
	OPTION_BACK
};

void optionsStart()
{
	page = 0;
	cursor = 0;
}

void optionsUpdate()
{
	int yaxis = inp_ButtonPressed(BTN_DOWN) - inp_ButtonPressed(BTN_UP);

	if (yaxis)
	{
		aud_PlaySound(sounds[sndPi01]);
		cursor += yaxis;
	}

	switch (page)
	{
		case 0:
		{
			#ifndef _NO_EXIT
			 if (cursor > 3) cursor = 0;
			 if (cursor < 0) cursor = 3;
			#else
			 if (cursor > 2) cursor = 0;
			 if (cursor < 0) cursor = 2;
			#endif

			//return to game
			if (inp_ButtonPressed(BTN_SELECT) || inp_ButtonPressed(BTN_DECLINE))
				gameSetState(GAME_STATE_GAME);

			//select option
			else if (inp_ButtonPressed(BTN_ACCEPT))
			{
				//continue
				if (cursor == 0)
				{
					gameSetState(GAME_STATE_GAME);
				}
				//options
				else if (cursor == 1)
				{
					page = 1;
					cursor = 0;
				}
				//reset
				else if (cursor == 2)
				{
					gameReset();
				}
				//quit
				else if (cursor == 3)
				{
					sys_QuitGame();
				}
			}			
		}
		break;

		case 1:
		{
			if (cursor > OPTION_BACK) cursor = 0;
			if (cursor < 0) cursor = OPTION_BACK;

			int xaxis = inp_ButtonPressed(BTN_RIGHT) - inp_ButtonPressed(BTN_LEFT);

			if (xaxis)
			{
				if (cursor == OPTION_LANGUAGE)
					loadText(gameGetLanguage() + sign(xaxis));

				#ifdef _HAS_VIDEOMODE
					if (cursor == OPTION_VIDEOMODE)
						gfx_SetVideoMode(gfx_GetVideoMode() + xaxis);
				#endif

				#ifdef _HAS_FULLSCREEN
					if (cursor == OPTION_FULLSCREEN)
						gfx_SetFullscreen(!gfx_GetFullscreen());
				#endif

				#ifdef _HAS_VSYNC
					if (cursor == OPTION_VSYNC)
						gfx_SetVsync(!gfx_GetVsync());
				#endif

				if (cursor == OPTION_MUSIC)
					aud_SetMusicVolume(aud_GetMusicVolume() + (10*xaxis));

				if (cursor == OPTION_SOUND)
				{
					aud_SetSoundVolume(aud_GetSoundVolume() + (10*xaxis));
					aud_PlaySound(sounds[sndPi01]);
				}

				if (cursor == OPTION_TIMER)
					gameSetTimer(!gameGetTimer());
			}

			if (inp_ButtonPressed(BTN_ACCEPT))
			{
				if (cursor == OPTION_LANGUAGE)
					loadText(gameGetLanguage() + 1);

				#ifdef _HAS_VIDEOMODE
					if (cursor == OPTION_VIDEOMODE)
						gfx_SetVideoMode(gfx_GetVideoMode() + 1);
				#endif

				#ifdef _HAS_FULLSCREEN
					if (cursor == OPTION_FULLSCREEN)
						gfx_SetFullscreen(!gfx_GetFullscreen());
				#endif

				#ifdef _HAS_VSYNC
					if (cursor == OPTION_VSYNC)
						gfx_SetVsync(!gfx_GetVsync());
				#endif

				if (cursor == OPTION_TIMER)
					gameSetTimer(!gameGetTimer());

				if (cursor == OPTION_BACK)
				{
					page = 0;
					cursor = 1;
					iniSave();
				}
			}

			//go back
			if (inp_ButtonPressed(BTN_DECLINE))
			{
				page = 0;
				cursor = 1;
				iniSave();
			}
		}
		break;
	}
}

void optionsDraw(float subFrame, float depth)
{
	switch (page)
	{
		case 0:
		{
			drawTextBold(TEXT_CONTINUE, 160,  72, TEXT_COLOR_YELLOW, true);
			drawTextBold(TEXT_OPTIONS,  160,  88, TEXT_COLOR_YELLOW, true);
			drawTextBold(TEXT_RESET,    160, 104, TEXT_COLOR_YELLOW, true);
			#ifndef _NO_EXIT
			drawTextBold(TEXT_EXIT,		160, 120, TEXT_COLOR_YELLOW, true);
			#endif

			drawTextBold("<         >", 160, 72+(16*cursor), TEXT_COLOR_RED, true);

			#ifdef _HAS_KEYBOARD
			drawTextBold(TEXT_CTRL_RESET, 160, 160, TEXT_COLOR_RED, true);
			drawTextBold(TEXT_ESC_EXIT,   160, 176, TEXT_COLOR_RED, true);
			#endif
		}
		break;

		case 1:
		{
			int x = 98;
			int x2 = x + 90;
			int y = 72;

			drawTextBold(">", x-16, y + (16*cursor), TEXT_COLOR_RED);

			for (int i = 0; i <= OPTION_BACK; i++)
			{
				if (i == OPTION_LANGUAGE)
				{
					drawTextBold(TEXT_LANGUAGE, x, y, TEXT_COLOR_YELLOW);
					drawTextBold(getLanguageLabel(gameGetLanguage()), x2, y, 2);
				}
				
			#ifdef _HAS_VIDEOMODE
				else if (i == OPTION_VIDEOMODE)
				{					
					drawTextBold(TEXT_VIDEO, x, y, TEXT_COLOR_YELLOW);
					char str[32];
					gfx_GetVideoMode(str);
					drawTextBold(str, x2, y, 2);
				}
			#endif

			#ifdef _HAS_FULLSCREEN
				else if (i == OPTION_FULLSCREEN)
				{		
					drawTextBold(TEXT_FULLSCREEN, x, y, TEXT_COLOR_YELLOW);
					drawTextBold((gfx_GetFullscreen())? TEXT_ON: TEXT_OFF, x2, y, TEXT_COLOR_YELLOW);
				}
			#endif

			#ifdef _HAS_VSYNC
				else if (i == OPTION_VSYNC)
				{
					drawTextBold(TEXT_VSYNC, x, y, TEXT_COLOR_YELLOW);
					drawTextBold((gfx_GetVsync())? TEXT_ON: TEXT_OFF, x2, y, TEXT_COLOR_YELLOW);
				}
			#endif

				else if (i == OPTION_MUSIC)
				{
					drawTextBold(TEXT_MUSIC, x, y, TEXT_COLOR_YELLOW);
					char str[8];
					sprintf(str, "%d", aud_GetMusicVolume());
					drawTextBold(str, x2, y, 2);
				}
				else if (i == OPTION_SOUND)
				{
					drawTextBold(TEXT_SOUND, x, y, 2);
					char str[8];
					sprintf(str, "%d", aud_GetSoundVolume());
					drawTextBold(str, x2, y, 2);
				}
				else if (i == OPTION_TIMER)
				{
					drawTextBold(TEXT_TIMER, x, y, TEXT_COLOR_YELLOW);
					drawTextBold((gameGetTimer())? TEXT_ON: TEXT_OFF, x2, y, TEXT_COLOR_YELLOW);
				}
				else if (i == OPTION_BACK)
				{
					drawTextBold(TEXT_BACK, x, y, 2);
				}

				y += 16;
			}
		}
		break;
	}
}