#pragma once

#include "graphics.hpp"
#include "audio.hpp"
#include "map.hpp"

void loadTitleAssets();
void loadAssets();
void freeAssets();

void loadTileset(int level);
void loadBossGraphics(int level);
void loadMusic(int level);

#define NUM_OF_IMAGES 15
#define NUM_OF_SOUNDS 44
#define NUM_OF_MUSIC 6

extern Surface* images[NUM_OF_IMAGES];
extern Sound* sounds[NUM_OF_SOUNDS];
extern Music* music[NUM_OF_MUSIC];

enum {
	imgChr20 = 0, 	//misc effects, weapons, and items
	imgChr2040,		//enemy tongue
	imgChr32,		//poof and stun effect, boulders
	imgChr6020,		//dragon statue flame
	imgChr64,		//bomb explosion
	imgDark,
	imgEne01,
	imgFont,
	imgFontKana,
	imgItems,
	imgMychr,		//hero character
	imgStatus,		//hud
	imgTitle01,
	imgTiles,
	imgBoss
};

enum {
	sndBee01 = 0,
	sndBell01,
	sndBom01,
	sndBom02,
	sndBom03,
	sndDoor00,
	sndFire01,
	sndGas01,
	sndGet01,
	sndGet02,
	sndHit01,
	sndHit02,
	sndHit03,
	sndHit04,
	sndHit05,
	sndHit06,
	sndHit07,
	sndJump01,
	sndJump02,
	sndNazo,
	sndNg,
	sndOk,
	sndPi01,
	sndPi02,
	sndPi03,
	sndPi04,
	sndPi05,
	sndPi06,
	sndPi07,
	sndPi08,
	sndPi09,
	sndPi10,
	sndPower01,
	sndPower02,
	sndShot01,
	sndShot02,
	sndShot03,
	sndShot04,
	sndShot05,
	sndShot06,
	sndShot07,
	sndStep01,
	sndWater01,
	sndWolf01
};

enum {
	bgmMusic,
	bgmBoss,
	bgmSecret,
	bgmGameOver,
	bgmStart,
	bgmClear
};