#include "hud.hpp"
#include "assets.hpp"
#include "math.hpp"
#include "hero.hpp"
#include "text.hpp"
#include "game.hpp"

static unsigned long hp = 128;
static unsigned long lasthp = 128;

void hudUpdate()
{
	lasthp = hp;

	if (hp != hero.getHp())
	{
		if (hp < hero.getHp())
			hp += 1;
		else
			hp -= 1;

		if (hp == 0)
			hero.kill();
	}		
}

void hudDraw(float subFrame, float depth)
{
	//main image
	int x = -5 * depth;
	int y = (hero.getY() > 52)? 8: 200;
	y += gfx_GetScrollY();
	
	gfx_DrawSurfacePart(images[imgStatus], x+4, y, 0, 0, 184, 32);

	//health bar
	Color hpcolor = (Color){ 128, 255, 0 };
	
	if (hero.isPoisoned())
		hpcolor = (Color){ 255, 128, 255 };

	unsigned long maxhp = hero.getMaxHp();
	float drawhp = lerp(hp, lasthp, subFrame);

	gfx_DrawRect(x+38, y+4, x+38 + maxhp, y+4+3, (Color){255,0,0});
	gfx_DrawRect(x+38, y+4, x+38 + drawhp, y+4+3, hpcolor);

	//weapon icon
	gfx_DrawSurfacePart(images[imgStatus], x+12, y+8, 16 * hero.getWeapon(), 32, 16, 16);

	//Ammo counter
	char num[8];
	sprintf(num, "%02ld", hero.getAmmo());
	drawBold(num, x+37, y + 18, TEXT_COLOR_WHITE);

	//time
	if (gameGetTimer())
	{
		unsigned long time = gameGetPlayTime();
		unsigned int seconds = time / 60;
		unsigned int minutes = seconds / 60;
		unsigned int hours = minutes / 60;

		minutes = minutes % 60;
		seconds = seconds % 60;

		char str[64];
		sprintf(str, "%02d:%02d:%02d", hours, minutes, seconds);
		drawBold(str, x+104, y+18, TEXT_COLOR_WHITE);
	}
}