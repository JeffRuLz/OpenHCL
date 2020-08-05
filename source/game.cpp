#include "game.hpp"
#include "input.hpp"
#include "system.hpp"
#include "assets.hpp"
#include "collision.hpp"
#include "credits.hpp"
#include "ev.hpp"
//#include "hero.hpp"
#include "hud.hpp"
#include "inventory.hpp"
#include "math.hpp"
#include "options.hpp"
#include "savedata.hpp"
#include "titlescreen.hpp"
#include "text.hpp"
#include "results.hpp"
#include "objects/block.hpp"
#include "effects/lava.hpp"
#include "stagedata.h"
#include <string.h>
#include <time.h>

Hero hero = Hero(0,0);

ObjectList enemyList = ObjectList();
ObjectList effectList = ObjectList();
ObjectList objectList = ObjectList();
ObjectList platformList = ObjectList();
ObjectList weaponList = ObjectList();

static SaveData* saveData = nullptr;
static GameState state = GAME_STATE_TITLESCREEN;
static int gameLanguage = LANG_JAPANESE;
static Tilemap* tilemap[2] = { nullptr };

static int level = 0;
static int screenX = 5;
static int screenY = 2;
static unsigned long playTime = 0;
static bool gameFlags[4500] = { false };

static int lastItem = 0;
static int doorMessage = 0;
static bool bossRoom = false;
static bool darkRoom = false;
static bool showTimer = false;
static bool ringBell = false;

static int warpx;
static int warpy;
static int warplevel;
static int warpscreen;

static int transitionTimer = 0;
static int curtainTimer = 120;
static int secretTimer = 0;
static int saveTimer = 0;
static int quakeTimer = 0;

static void _parseMap()
{
	for (int y = 0; y < 18; y++)
	{
		for (int x = 0; x < 22; x++)
		{
			int id = x + y * 22;
			int tile = map[1].data[id];

			//create blocks
			if (tile == 176 || tile == 177)
			{
				platformList.add(new Block((x-3)*20, (y-3)*20));
				map[1].data[id] = 0;
			}

			//move decorations to background
			else if (tile == 68 || tile == 35 || tile == 36 || tile == 51 || tile == 52)
			{
				if (level == 0)
				{
					map[0].data[id] = map[1].data[id];
					map[1].data[id] = 0;
				}
			}

			//lava top
			else if (tile == 18)
			{
				effectList.add(new Lava((x-3)*20, (y-3)*20, 0));
				map[1].data[id] = 0;
			}

			//water top
			else if (tile == 22)
			{
				effectList.add(new Lava((x-3)*20, (y-3)*20, 1));
				map[1].data[id] = 0;
			}

			tile = map[0].data[id];
			if (tile == 151)
			{
				map[0].data[id] = 34;
				map[1].data[id] = 46;
			}

			if (tile == 152)
			{
				map[0].data[id] = 34;
				map[1].data[id] = 47;
			}
		}
	}
}

static void changeScreen(int x, int y)
{
	darkRoom = false;

	if (bossRoom)
	{
		aud_StopMusic();
		aud_PlayMusic(music[bgmMusic], true);
		bossRoom = false;
	}

	//reset temp flags
	memset(&gameFlags[4000], false, 500);

	//unload objects
	enemyList.clear();
	effectList.clear();
	objectList.clear();
	platformList.clear();
	weaponList.clear();

	hero.setCanDoubleJump(true);
	
	if (hero.getState() == HERO_STATE_HIT || hero.getState() == HERO_STATE_STONE || hero.getState() == HERO_STATE_CHARGE)
		hero.setState(HERO_STATE_NORMAL);

	//load map file
	int id = roomIDs[level][x + y * 12];

	loadMap(id, true);
	loadMap(id, false);

	//load objects
	loadEv(id);

	_parseMap();

	//dark rooms are hard coded supposedly
	darkRoom = ((id >= 165 && id <= 172) || id == 174);

	//upload map data to graphics engine
	gfx_FreeTilemap(tilemap[0]);
	gfx_FreeTilemap(tilemap[1]);
	tilemap[0] = gfx_LoadTilemap(images[imgTiles], 22, 18, map[0].data);
	tilemap[1] = gfx_LoadTilemap(images[imgTiles], 22, 18, map[1].data);

	if (ringBell)
	{
		aud_PlaySound(sounds[sndBell01]);
		ringBell = false;
	}
}

void gameStart()
{
	srand(time(0));

	saveData = createSaveData();
	state = GAME_STATE_TITLESCREEN;

	loadTitleAssets();
	//loadAssets();
	//loadText(gameLanguage);

	gameResetGame();
	titleScreenStart();
}

void gameEnd()
{
	aud_StopAll();

	enemyList.clear();
	effectList.clear();
	objectList.clear();
	platformList.clear();
	weaponList.clear();

	gfx_FreeTilemap(tilemap[0]); tilemap[0] = nullptr;
	gfx_FreeTilemap(tilemap[1]); tilemap[1] = nullptr;	

	freeAssets();
	freeSaveData(saveData);
	saveData = nullptr;

	#ifndef _DEBUG
	sys_DeleteSaveFile(SYS_TEMPSAVEPATH);
	#endif
}

void gameReset()
{
	#ifndef _DEBUG
	sys_DeleteSaveFile(SYS_TEMPSAVEPATH);
	#endif

	gameSetState(GAME_STATE_TITLESCREEN);
}

void gameResetGame()
{
	enemyList.clear();
	effectList.clear();
	objectList.clear();
	platformList.clear();
	weaponList.clear();

	level = 0;
	screenX = 5;
	screenY = 2;
	playTime = 0;
	memset(gameFlags, false, 4500);

	loadTileset(level);
	loadBossGraphics(level);
	loadMusic(level);

	bossRoom = false;
	transitionTimer = 0;
	curtainTimer = 120;
	secretTimer = 0;
	saveTimer = 0;
	quakeTimer = 0;

	hero = Hero(150, 170);
}

void gameLoadGame()
{
	gameResetGame();

	//try loading temp save
	if (sys_ReadSaveFile(SYS_TEMPSAVEPATH, saveData))
		applySaveData(saveData);
	//load normal save
	else if (sys_ReadSaveFile(SYS_SAVEPATH, saveData))
		applySaveData(saveData);

	loadTileset(level);
	loadBossGraphics(level);
	loadMusic(level);
}

void gameUpdate()
{
	//resume regular music after secret music
	if (secretTimer > 0 && state != GAME_STATE_TITLESCREEN)
	{
		if ((secretTimer -= 1) <= 0)
		{
			if (!bossRoom)
				aud_MuteMusic(false);
		}
	}

	//non-game states
	if (state == GAME_STATE_TITLESCREEN)
	{
		titleScreenUpdate();
		return;
	}
	else if (state == GAME_STATE_INVENTORY)
	{
		inventoryUpdate();
		return;
	}
	else if (state == GAME_STATE_OPTIONS)
	{
		optionsUpdate();
		return;
	}
	else if (state == GAME_STATE_RESULTS)
	{
		resultsUpdate();
		return;
	}
	else if (state == GAME_STATE_CREDITS)
	{
		creditsUpdate();
		return;
	}
	else if (state == GAME_STATE_TRANSITION)
	{
		transitionTimer -= 1;
		if (transitionTimer <= 0)
		{
			state = GAME_STATE_GAME;
			level = warplevel;
			loadTileset(level);
			loadBossGraphics(level);
			loadMusic(level);
			hero.warpX(warpx);
			hero.warpY(warpy);
			screenX = warpscreen % 12;
			screenY = warpscreen / 12;
			bossRoom = false;
			aud_PlayMusic(music[bgmMusic], true);
		}

		return;
	}

	//mid screen transition
	if (transitionTimer < 10)
	{
		transitionTimer += 1;

		if (transitionTimer == 9)
			changeScreen(screenX, screenY);

		else if (transitionTimer == 10)
		{
			//save backup file between rooms
			//can be disabled on systems with slow write speeds
			#if !defined(_NO_TEMP_SAVE) || defined(_DEBUG)
			updateSaveData(saveData);
			sys_WriteSaveFile(SYS_TEMPSAVEPATH, (void*)saveData);	
			#endif				
		}

		return;		
	}

	//door hint
	if (doorMessage != 0)
	{
		hudUpdate();

		if (inp_ButtonPressed(BTN_ATTACK) || inp_ButtonPressed(BTN_JUMP) || inp_ButtonPressed(BTN_WEAPON))
			doorMessage = 0;

		return;
	}

	//save message
	if (state == GAME_STATE_SAVE)
	{
		hudUpdate();
		objectList.update();

		saveTimer -= 1;

		if (saveTimer == 30)
		{
			updateSaveData(saveData);
			sys_WriteSaveFile(SYS_SAVEPATH, (void*)saveData);
			sys_DeleteSaveFile(SYS_TEMPSAVEPATH);
		}

		if (saveTimer <= 0)
			state = GAME_STATE_GAME;

		return;
	}

	//open menus
	if (hero.getState() != HERO_STATE_DEATH && hero.getState() != HERO_STATE_ITEM_GET && state != GAME_STATE_GAME_OVER)
	{
		if (inp_ButtonPressed(BTN_START))
		{
			aud_PlaySound(sounds[sndPi04]);
			state = GAME_STATE_INVENTORY;
			return;
		}

		else if (inp_ButtonPressed(BTN_SELECT))
		{
			state = GAME_STATE_OPTIONS;
			optionsStart();
			return;
		}
	}

	//curtain transition
	if (curtainTimer > 0)
	{				
		if ((curtainTimer -= 4) <= 0)
		{
			if (!bossRoom)
				aud_PlayMusic(music[bgmMusic], true);
			curtainTimer = 0;
		}
	}

	//earthquake
	if (quakeTimer > 0)
	{
		switch (quakeTimer % 4)
		{
			case 0: gfx_SetScrollY(-1); break;
			case 2: gfx_SetScrollY(1); break;
			default: gfx_SetScrollY(0);
		}

		if ((quakeTimer -= 1) <= 0)
		{
			gfx_SetScrollY(0);
		}
	}

	//main game
	playTime += 1;

	//update objects
	hero.update();
	hudUpdate();

	if (hero.getState() == HERO_STATE_ITEM_GET)
	{
		return;
	}
	else
	{
		if (lastItem != 0)
		{
			gameSetFlag(lastItem, true);
			lastItem = 0;
		}
	}

	platformList.update();
	objectList.update();
	weaponList.update();
	enemyList.update();
	effectList.update();

	//screen transition
	if (hero.getX() >= 330)
	{
		hero.warpX(10);
		screenX += 1;
		transitionTimer = 0;
	}
	else if (hero.getX() <= -10)
	{
		hero.warpX(310);
		screenX -= 1;
		transitionTimer = 0;
	}
	else if (hero.getY() >= 250)
	{
		hero.warpY(10);
		hero.setVsp(0);
		screenY += 1;
		transitionTimer = 0;
	}
	else if (hero.getY() <= -10 && hero.getState() == HERO_STATE_LADDER)
	{
		hero.warpY(230);
		screenY -= 1;
		transitionTimer = 0;
	}	
}

void gameDraw(float subFrame, float depth)
{
	float sf = subFrame;

	//non-game states
	if (state == GAME_STATE_TITLESCREEN)
	{
		titleScreenDraw(sf, depth);
		return;
	}
	else if (state == GAME_STATE_INVENTORY)
	{
		inventoryDraw(sf, depth);
		return;
	}
	else if (state == GAME_STATE_OPTIONS)
	{
		optionsDraw(sf, depth);
		return;
	}
	else if (state == GAME_STATE_CREDITS)
	{
		creditsDraw(sf, depth);
		return;
	}
	else if (state == GAME_STATE_TRANSITION)
	{
		gfx_DrawRect(0, 0, 320, 240, (Color){0,0,0} );

		if (warplevel != 0)
		{
			int msg = TEXT_DUNGEON + warplevel-1;
			
			if (warplevel == 5) msg += 1;
			if (warplevel == 6) msg -= 1;

			drawText(msg, 160, 108, true);
		}

		return;
	}

	//black screen during screen transition
	if (transitionTimer < 10)
	{
		gfx_DrawRect(0, 0, 320, 240, (Color){0,0,0} );
		return;
	}

	//disable interpolation in certain situations
	if (state == GAME_STATE_SAVE ||
		state == GAME_STATE_RESULTS ||
		hero.getState() == HERO_STATE_ITEM_GET ||
		doorMessage != 0)
	{
		sf = 1;
	}

	//main game
	gfx_DrawTilemap(tilemap[0], 8*depth, 0);
	gfx_DrawTilemap(tilemap[1], 0, 0);

	platformList.draw(sf, depth);
	effectList.draw(sf, depth, 0);
	objectList.draw(sf, depth);
	effectList.draw(sf, depth, 1);
	enemyList.draw(sf, depth, 0);
	enemyList.draw(sf, depth, 1);
	weaponList.draw(sf, depth);				
	hero.draw(sf, depth);
	effectList.draw(sf, depth, 2);

	if (darkRoom)
	{
		float hx = lerp(hero.getX(), hero.getPrevX(), sf);
		float hy = lerp(hero.getY(), hero.getPrevY(), sf);

		hx -= 4 * depth;

		float left 	 = hx - 80;
		float right  = hx + 80;
		float top 	 = hy - 80;
		float bottom = hy + 80;

		Color col = (Color){ 10, 0 ,0 };
		gfx_DrawRect(0,0,left+0.1,240,col);
		gfx_DrawRect(right-0.1,0,321,241,col);
		gfx_DrawRect(left-0.1,0,right+0.1,top+0.1,col);
		gfx_DrawRect(left-0.1,bottom-0.5,right+0.1,241,col);

		gfx_DrawTile(images[imgDark], left, top, 160, 160, gameHasItem(ITEM_CANDLE));
	}

	if (state != GAME_STATE_RESULTS)
		hudDraw(sf, depth);
	else
		resultsDraw(subFrame, depth);

	//draw curtain effect
	if (curtainTimer > 0)
	{
		float curtainLerp = lerp(curtainTimer, curtainTimer+4, sf);

		gfx_DrawRect(0, 0, 320, curtainLerp, (Color){0,0,0} );
		gfx_DrawRect(0, 240-curtainLerp, 320, 240, (Color){0,0,0} );
	}

	//save message
	if (state == GAME_STATE_SAVE)
	{
		float rx = 70-8*depth;
		float ry = 104;
		gfx_DrawRect(rx, ry, rx+180, ry+32, (Color){ 0xff, 0xff, 0xff });
		gfx_DrawRect(rx+1, ry+1, rx+179, ry+31, (Color){ 0x00, 0x00, 0xff });
		drawText(TEXT_SAVING, 160-8*depth, 108, true);
	}

	//item message
	else if (hero.getState() == HERO_STATE_ITEM_GET && hero.getImageIndex() >= 3)
	{
		int item = lastItem - 2000 + 1;
		float rx = 70-8*depth;
		float ry = 104;
		//large box
		gfx_DrawRect(rx, ry, rx+180, ry+32, (Color){ 0xff, 0xff, 0xff });
		gfx_DrawRect(rx+1, ry+1, rx+179, ry+31, (Color){ 0x00, 0x14, 0x00});
		//small box
		gfx_DrawRect(rx+4, ry+4, rx+28, ry+28, (Color){ 0xff, 0xff, 0xff });
		//image
		gfx_DrawTile(images[imgItems], rx+6, ry+6, 20, 20, item);
		//text
		bool twolines = false;
		int textx = rx + 28;
		int texty = ry + 5;

		if (messageLength(TEXT_ITEM_NAMES+item-1) + 2 + messageLength(TEXT_FOUND) > 16)
		{
			twolines = true;
			texty -= 4;
		}

		textx = drawChar('[', textx, texty);
		textx = drawText(TEXT_ITEM_NAMES + item - 1, textx, texty);
		textx = drawChar(']', textx, texty);

		if (twolines)
		{
			textx = rx + 28;
			texty += 10;
		}

		drawText(TEXT_FOUND, textx, texty);
	}

	//door hint
	else if (doorMessage != 0)
	{
		float rx = 60-8*depth;
		float ry = 104;

		int img = 34 + doorMessage;
		int msg = TEXT_DUNGEON + (doorMessage-1);
		if (doorMessage == 5) { img += 1; msg += 1; }
		if (doorMessage == 6) { img -= 1; msg -= 1; }

		//large box
		gfx_DrawRect(rx, ry, rx+200, ry+32, (Color){ 0xff, 0xff, 0xff });
		gfx_DrawRect(rx+1, ry+1, rx+199, ry+31, (Color){ 0x00, 0x14, 0x00});
		//small box
		gfx_DrawRect(rx+4, ry+4, rx+28, ry+28, (Color){ 0xff, 0xff, 0xff });
		//image
		gfx_DrawTile(images[imgItems], rx+6, ry+6, 20, 20, img);
		//text
		int textx = rx + 30;
		int texty = ry + 5;
		drawText(msg, textx, texty);
	}
}

void gameItemMessage(int item)
{
	state = GAME_STATE_ITEM;
}

void gameDoorMessage(int n)
{
	doorMessage = n;
}

bool gameHasItem(int n)
{
	return gameFlags[2006 + n];
}

bool gameHasWeapon(int n)
{
	if (n <= 0 || n > 5)
		return false;

	return gameFlags[2000 + n - 1];
}

bool gameHasKey(int n)
{
	return gameFlags[2034 + n];
}

//
GameState gameGetState()
{
	return state;
}

void gameSetState(GameState s)
{
	if (s == GAME_STATE_TITLESCREEN)
		titleScreenStart();

	else if (s == GAME_STATE_RESULTS)
		resultsStart();

	else if (s == GAME_STATE_CREDITS)
		creditsStart();

	else if (s == GAME_STATE_SAVE)
		saveTimer = 60;

	else if (s == GAME_STATE_TRANSITION)
	{
		if (warplevel != 0)
		{
			aud_PlayMusic(music[bgmStart], false);
			transitionTimer = 120;
		}
	}

	state = s;
}

int gameGetLanguage()
{
	return gameLanguage;
}

void gameSetLanguage(int l)
{
	gameLanguage = l;
}

int gameGetLevel()
{
	return level;
}

void gameSetLevel(int n)
{
	level = n;
}

int gameGetScreen()
{
	return screenX + (screenY * 12);
}

void gameSetScreen(int n)
{
	screenX = n % 12;
	screenY = n / 12;
}

unsigned long gameGetPlayTime()
{
	return playTime;
}

void gameSetPlayTime(unsigned long n)
{
	playTime = n;
}

bool gameGetFlag(int n)
{
	if (n == 0)
		return false;
	
	return gameFlags[n];
}

void gameSetFlag(int n, bool on)
{
	if (n != 0)
		gameFlags[n] = on;
}

void gameSetLastItem(int n)
{
	lastItem = n;
}

void gameSetBossRoom(bool on)
{
	bossRoom = on;
}

bool gameGetTimer()
{
	return showTimer;
}

void gameSetTimer(bool on)
{
	showTimer = on;
}

void gameSetWarp(int x, int y, int lvl, int screen)
{
	warpx = x;
	warpy = y;
	warplevel = lvl;
	warpscreen = screen;
}

void gameRingBell()
{
	if (gameHasItem(ITEM_BELL))
		ringBell = true;
}

void gamePlaySecret()
{
	aud_MuteMusic(true);
	
	int temp = aud_GetSoundVolume();
	aud_SetSoundVolume(aud_GetMusicVolume());
	aud_PlaySound(sounds[sndNazo]);
	aud_SetSoundVolume(temp);

	secretTimer = 180;
}

int gameGetSecretTimer()
{
	return secretTimer;
}

int gameGetQuake()
{
	return quakeTimer;
}

void gameSetQuake(int n)
{
	quakeTimer = n;
}