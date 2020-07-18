#include "inventory.hpp"
#include "assets.hpp"
#include "input.hpp"
#include "game.hpp"
#include "text.hpp"
#include "hero.hpp"

static int cursorX = 0;
static int cursorY = 0;

void inventoryUpdate()
{
	hero.changeWeapon();
	
	int xaxis = inp_ButtonPressed(BTN_RIGHT) - inp_ButtonPressed(BTN_LEFT);
	int yaxis = inp_ButtonPressed(BTN_DOWN) - inp_ButtonPressed(BTN_UP);

	if (xaxis != 0 || yaxis != 0)
	{
		aud_PlaySound(sounds[sndPi01]);

		cursorX += xaxis;
		cursorY += yaxis;

		if (cursorX < 0) cursorX = 6;
		if (cursorX > 6) cursorX = 0;
		if (cursorY < 0) cursorY = 3;
		if (cursorY > 3) cursorY = 0;
	}

	if (inp_ButtonPressed(BTN_START))
		gameSetState(GAME_STATE_GAME);
}

void inventoryDraw(float subFrame, float depth)
{
	if (gameGetState() == GAME_STATE_TITLESCREEN || gameGetState() == GAME_STATE_CREDITS)
		return;

	int tile = 0;
	int i = 0;

	//labels
	drawTextBold(TEXT_SUB_WEAPON, 8, 8, TEXT_COLOR_YELLOW);
	drawTextBold(TEXT_ITEM, 8, 48, TEXT_COLOR_YELLOW);
	drawTextBold(TEXT_KEY, 8, 160, TEXT_COLOR_YELLOW);

	//weapons
	for (i = 0; i < 5; i++)
	{
		if (gameHasWeapon(i+1))
			tile = i + 1;
		else
			tile = 0;

		gfx_DrawRect(9+(24*i), 17, 9+(24*i)+22, 17+22, (Color){ 0xFF,0xFF,0xFF });
		gfx_DrawTile(images[imgItems], 10+(24*i), 18, 20, 20, tile);
	}

	int w = hero.getWeapon();
	if (w != 0)
		gfx_DrawSurfacePart(images[imgStatus], 8+((w-1)*24), 16, 0, 48, 24, 24);

	//items
	int itemOrder[28] = {
		ITEM_COPPER_PICK, ITEM_SILVER_PICK, ITEM_BELL, ITEM_GOLD_GEM, ITEM_POWER_BAND, ITEM_FIN, ITEM_GILL,
		ITEM_GAS_MASK, ITEM_PURPLE_RING, ITEM_GREEN_RING, ITEM_CAPE, ITEM_CATFISH_GEM, ITEM_BLUE_BOOTS, ITEM_RED_BOOTS,
		ITEM_SHIELD, ITEM_GREEN_SCROLL, ITEM_BLUE_SCROLL, ITEM_RED_SCROLL, ITEM_CANDLE, ITEM_BLUE_KEY, ITEM_RED_KEY,
		ITEM_CROSS, ITEM_DRAGON_IDOL, ITEM_BLUE_COIN, ITEM_RED_COIN, ITEM_LEFT_COIN, ITEM_RIGHT_COIN, ITEM_ROC_FEATHER
	};

	i = 0;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 7; x++)
		{
			if (gameHasItem(itemOrder[i]))
				tile = 7 + itemOrder[i];
			else
				tile = 0;

			gfx_DrawRect(9+(24*x), 57+(24*y), 9+(24*x)+22, 57+(24*y)+22, (Color){ 0xFF,0xFF,0xFF });
			gfx_DrawTile(images[imgItems], 10+(24*x), 58+(24*y), 20, 20, tile);

			i += 1;
		}
	}

	//cursor
	gfx_DrawSurfacePart(images[imgStatus], 8+(cursorX*24), 56+(cursorY*24), 0, 48, 24, 24);

	//keys
	for (i = 0; i < 8; i++)
	{
		if (gameHasKey(i))
			tile = 35 + i;
		else
			tile = 0;

		gfx_DrawRect(9+(24*i), 169, 9+(24*i)+22, 169+22, (Color){ 0xFF, 0xFF, 0xFF });
		gfx_DrawTile(images[imgItems], 10+(24*i), 170, 20, 20, tile);
	}

	//text box
	gfx_DrawRect(8, 200, 8+303, 200+23, (Color){ 0xFF, 0xFF, 0xFF });
	gfx_DrawRect(9, 201, 9+301, 201+21, (Color){ 0x00, 0x00, 0xFF });

	int cursorItem = itemOrder[cursorX + (cursorY * 7)];
	if (gameHasItem(cursorItem))
	{
		bool twolines = false;
		int textx = 16;
		int texty = 200;

		if (messageLength(TEXT_ITEM_NAMES + 6 + cursorItem) + messageLength(TEXT_ITEM_DESC + cursorItem) + 1 > 33)
		{
			texty -= 5;
			twolines = true;
		}

		textx = drawText(TEXT_ITEM_NAMES + 6 + cursorItem, textx, texty);
		textx = drawChar(':', textx, texty);

		if (twolines)
		{
			textx = 16;
			texty += 10;
		}

		drawText(TEXT_ITEM_DESC + cursorItem, textx, texty);
	}
}

#ifdef _3DS
void inventoryTouch()
{
	int tx = inp_GetTouchX();
	int ty = inp_GetTouchY();

	if (tx <= 0 && ty <= 0)
		return;

	if (gameGetState() == GAME_STATE_TITLESCREEN ||
		gameGetState() == GAME_STATE_GAME_OVER || 
		gameGetState() == GAME_STATE_RESULTS || 
		gameGetState() == GAME_STATE_CREDITS)
		return;

	//change weapon
	for (int i = 1; i <= 5; i++)
	{
		if (tx < 9 + ((i-1)*24) || tx > 9 + ((i-1)*24) + 24 || ty < 17 || ty > 39)
			continue;
		else
		{
			if (i != hero.getWeapon() && gameHasWeapon(i))
			{
				hero.setWeapon(i);
				aud_PlaySound(sounds[sndPi01]);
			}

			break;
		}
	}

	//highlight item
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 7; x++)
		{
			if (tx < 9 + x * 24 || tx > 9 + x * 24 + 24 ||
				ty < 57 + y * 24 || ty > 57 + y * 24 + 24)
				continue;
			else
			{
				if (cursorX != x || cursorY != y)
				{
					aud_PlaySound(sounds[sndPi01]);
					cursorX = x;
					cursorY = y;
				}

				break;
			}
		}
	}
}
#endif