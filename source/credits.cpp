#include "credits.hpp"
#include "system.hpp"
#include "assets.hpp"
#include "math.hpp"
#include "text.hpp"
#include "game.hpp"
#include <string.h>

static char credits[][40] =
{
	"&TEXT_STAFF", //- ORIGINAL GAME STAFF -
	"",
	"",
	"",
	"",
	"&TEXT_SPRITES", //SPRITES
	"BUSTER",
	"",
	"",
	"&TEXT_PROGRAM", //PROGRAM
	"BUSTER",
	"",
	"",
	"&TEXT_MUSIC", //MUSIC
	"MATAJUUROU",
	"",
	"",
	"&TEXT_TEST_PLAYER", //TEST PLAYER
	"ZAC",
	"",
	"",
	"",
	"",
	"&TEXT_SPECIAL_THANKS", //- SPECIAL THANKS -
	"",
	"",
	"",
	"",
	"QUADRUPLE D",
	"SANDMAN",
	"",
	"",
	"KBGM",
	"KR.SHIN",
	"",
	"",
	"KBGMPLAYER",
	"NARUTO",
	"",
	"",
	"&TEXT_SOUND_EFFECTS", //SOUND EFFECTS
	"OSABISHIYUUKI",
	"",
	"",
	"EDGE",
	"TAKABO",
	"",
	"",
	"",
	"",
	"- OPEN HCL -",
	"",
	"",
	"",
	"",
	"&TEXT_PROGRAM", //PROGRAM
	"JEFFRULZ",
	"",
	"",
	"&TEXT_ENGLISH_TRANSLATION", //ENGLISH TRANSLATION
	"GARY THE KRAMPUS",
	"",
	"",
	"&TEXT_KOREAN_TRANSLATION", //KOREAN TRANSLATION
	"DDINGHOYA",
	"",
	"",

#ifdef _3DS
	"3DS VERSION",
	"JEFFRULZ",
	"LIB CTRU",
	"LIB CITRO3D",
	"LIB CITRO2D",
	"LIB OGG",
	"LIB VORBIS",
#endif

#ifdef _PSP
	"PSP VERSION",
	"JEFFRULZ",
	"PSPSDK",
	"LIB OGG",
	"LIB VORBIS",
#endif

#ifdef _WII
	"WII VERSION",
	"JEFFRULZ",
	"LIB OGC",
	"LIB OGG",
	"LIB VORBIS",
#endif

#ifdef _WIN32
	"PC VERSION",
	"JEFFRULZ",
	"SDL2",
	"SDL MIXER",
#endif

	"*" //end marker
};

static float scroll[2] = { -240 };
static const float scrollspd = 0.6;
static int numOfLines = 0;
static float heroImage = 0;
static int timer = 0;

void creditsStart()
{
	scroll[0] = scroll[1] = -272;
	heroImage = 0;
	timer = 0;
	
	for (int i = 0; i < 256; i++)
	{
		if (strcmp(credits[i], "*") == 0)
		{
			numOfLines = i;
			break;
		}
	}

	aud_StopMusic();
	aud_FreeMusic(music[bgmMusic]);
	music[bgmMusic] = aud_LoadMusic("ending");
	aud_PlayMusic(music[bgmMusic], false);
}

void creditsUpdate()
{
	scroll[1] = scroll[0];
	scroll[0] = min(scroll[0] + scrollspd, numOfLines * 8 + 142);
	heroImage = fmod(heroImage + 0.1, 2);

	if ((timer += 1) > 2230)
	{
		aud_StopMusic();
		gameReset();
	}
}

void creditsDraw(float subFrame, float depth)
{
	float dy = lerp(scroll[0], scroll[1], subFrame);
	float y = 8.f - fmod(dy, 8) - 16.f;
	char textCol = TEXT_COLOR_YELLOW;
	const char* line;
	int i = dy / 8.f;

	for (int a = 0; a < 32; a++)
	{
		if (i >= 0 && i < numOfLines)
		{
			line = credits[i];

			if (strcmp(line, "") == 0)
			{
				textCol = TEXT_COLOR_YELLOW;
			}
			else
			{
				if (line[0] != '&')
				{
					drawTextBold(line, 160, y, textCol, true);
				}
				else
				{
					if (strcmp(line, "&TEXT_STAFF") == 0)
						drawTextBold(TEXT_STAFF, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_SPRITES") == 0)
						drawTextBold(TEXT_SPRITES, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_PROGRAM") == 0)
						drawTextBold(TEXT_PROGRAM, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_MUSIC") == 0)
						drawTextBold(TEXT_MUSIC, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_TEST_PLAYER") == 0)
						drawTextBold(TEXT_TEST_PLAYER, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_SPECIAL_THANKS") == 0)
						drawTextBold(TEXT_SPECIAL_THANKS, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_SOUND_EFFECTS") == 0)
						drawTextBold(TEXT_SOUND_EFFECTS, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_ENGLISH_TRANSLATION") == 0)
						drawTextBold(TEXT_ENGLISH_TRANSLATION, 160, y, textCol, true);
					else if (strcmp(line, "&TEXT_KOREAN_TRANSLATION") == 0)
						drawTextBold(TEXT_KOREAN_TRANSLATION, 160, y, textCol, true);
				}

				textCol = TEXT_COLOR_WHITE;
			}
		}

		i += 1;
		y += 8.f;
	}

	y = numOfLines * 8 + 240 - dy;

	if (y < 240)
	{
		drawTextBold(TEXT_THE_END, 160, y, TEXT_COLOR_YELLOW, true);
		gfx_DrawSurfacePart(images[imgMychr], 150, y+16, 20 * (int)heroImage, 140, 20, 40);
	}
}