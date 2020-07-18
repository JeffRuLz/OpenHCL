#include "credits.hpp"
#include "system.hpp"
#include "assets.hpp"
#include "math.hpp"
#include "text.hpp"
#include "game.hpp"
#include <string.h>

static char credits[][40] =
{
	"- ORIGINAL GAME STAFF -", //0
	"",
	"",
	"",
	"",
	"SPRITES", //5
	"BUSTER",
	"",
	"",
	"PROGRAM", //9
	"BUSTER",
	"",
	"",
	"MUSIC", //13
	"MATAJUUROU",
	"",
	"",
	"TEST PLAYER", //17
	"ZAC",
	"",
	"",
	"",
	"",
	"- SPECIAL THANKS -", //23
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
	"SOUND EFFECTS", //40
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
	"PROGRAM", //55
	"JEFFRULZ",
	"",
	"",
	"ENGLISH TRANSLATION", //59
	"GARY THE KRAMPUS",
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

	//translation
	sprintf(credits[0], "%s", getText(TEXT_STAFF));
	sprintf(credits[5], "%s", getText(TEXT_SPRITES));
	sprintf(credits[9], "%s", getText(TEXT_PROGRAM));
	sprintf(credits[13], "%s", getText(TEXT_MUSIC));
	sprintf(credits[17], "%s", getText(TEXT_TEST_PLAYER));
	sprintf(credits[23], "%s", getText(TEXT_SPECIAL_THANKS));
	sprintf(credits[40], "%s", getText(TEXT_SOUND_EFFECTS));
	sprintf(credits[55], "%s", getText(TEXT_PROGRAM));
	sprintf(credits[59], "%s", getText(TEXT_ENGLISH_TRANSLATION));
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
				drawTextBold(line, 160, y, textCol, true);
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