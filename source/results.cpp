#include "results.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "text.hpp"
#include "math.hpp"

static unsigned int hours = 0;
static unsigned int minutes = 0;
static unsigned int seconds = 0;
static int treasure = 0;

static int timer = 0;
static int curtain = 0;

void resultsStart()
{
	//time
	unsigned long time = gameGetPlayTime();
	seconds = time / 60;
	minutes = seconds / 60;
	hours = minutes / 60;

	seconds = seconds % 60;
	minutes = minutes % 60;

	//item percentage
	int maxItems = 41;
	int items = 0;

	for (int i = ITEM_BLUE_SCROLL; i <= ITEM_BLUE_COIN; i++)
		items += gameHasItem(i);

	for (int i = WEAPON_ARROW; i <= WEAPON_BOMB; i++)
		items += gameHasWeapon(i);

	for (int i = 0; i < 8; i++)
		items += gameHasKey(i);

	treasure = ((float)items / (float)maxItems * 100.f);

	timer = 0;
	curtain = 0;

	aud_StopMusic();
	aud_PlayMusic(music[bgmClear], false);
}

void resultsUpdate()
{
	timer += 1;

	if (timer >= 500)
		curtain += 4;

	if (timer >= 530)
		gameSetState(GAME_STATE_CREDITS);
}

void resultsDraw(float subFrame, float depth)
{
	char str[32];

	drawTextBold(TEXT_ALL_CLEAR, 160, 32, TEXT_COLOR_YELLOW, true);
	
	drawTextBold(TEXT_TIME, 160, 64, TEXT_COLOR_YELLOW, true);
	sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds);
	drawTextBold(str, 160, 72, TEXT_COLOR_WHITE, true);

	drawTextBold(TEXT_TREASURE, 160, 96, TEXT_COLOR_YELLOW, true);
	sprintf(str, "%02d%%", treasure);
	drawTextBold(str, 160, 104, TEXT_COLOR_WHITE, true);

	//curtain
	if (timer >= 500)
	{
		float curtainLerp = lerp(curtain, curtain-4, subFrame);

		gfx_DrawRect(0, 0, 320, curtainLerp, (Color){0,0,0} );
		gfx_DrawRect(0, 240-curtainLerp, 320, 240, (Color){0,0,0} );
	}
}