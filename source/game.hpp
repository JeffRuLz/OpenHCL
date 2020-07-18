#pragma once

#include "system.hpp"
#include "object.hpp"
#include "hero.hpp"

extern Hero hero;

extern ObjectList enemyList;
extern ObjectList effectList;
extern ObjectList objectList;
extern ObjectList platformList;
extern ObjectList weaponList;

enum GameState {
	GAME_STATE_TITLESCREEN,
	GAME_STATE_GAME,
	GAME_STATE_SAVE,
	GAME_STATE_INVENTORY,
	GAME_STATE_OPTIONS,
	GAME_STATE_ITEM,
	GAME_STATE_GAME_OVER,
	GAME_STATE_TRANSITION,
	GAME_STATE_RESULTS,
	GAME_STATE_CREDITS
};

enum {
	ITEM_BLUE_SCROLL = 0,
	ITEM_RED_SCROLL,
	ITEM_BLUE_BOOTS,
	ITEM_PURPLE_RING,
	ITEM_CAPE,
	ITEM_GREEN_RING,
	ITEM_CATFISH_GEM,
	ITEM_POWER_BAND,
	ITEM_GILL,
	ITEM_FIN,
	ITEM_RED_BOOTS,
	ITEM_GREEN_SCROLL,
	ITEM_COPPER_PICK,
	ITEM_SHIELD,
	ITEM_GOLD_GEM,
	ITEM_BELL,
	ITEM_SILVER_PICK,
	ITEM_GAS_MASK,
	ITEM_RIGHT_COIN,
	ITEM_LEFT_COIN,
	ITEM_CROSS,
	ITEM_CANDLE,
	ITEM_RED_COIN,
	ITEM_ROC_FEATHER,
	ITEM_DRAGON_IDOL,
	ITEM_BLUE_KEY,
	ITEM_RED_KEY,
	ITEM_BLUE_COIN
};

enum {
	WEAPON_NONE = 0,
	WEAPON_ARROW,
	WEAPON_AXE,
	WEAPON_BOOMERANG,
	WEAPON_FIREBALL,
	WEAPON_BOMB
};

void gameStart();
void gameEnd();

void gameReset();

void gameResetGame();
void gameLoadGame();

void gameUpdate();
void gameDraw(float subFrame, float depth);

void gameItemMessage(int item);
void gameDoorMessage(int n);

//
bool gameHasItem(int n);
bool gameHasWeapon(int n);
bool gameHasKey(int n);

//
GameState gameGetState();
void gameSetState(GameState s);

int gameGetLanguage();
void gameSetLanguage(int l);

int gameGetLevel();
void gameSetLevel(int n);

int gameGetScreen();
void gameSetScreen(int n);

unsigned long gameGetPlayTime();
void gameSetPlayTime(unsigned long n);

bool gameGetFlag(int n);
void gameSetFlag(int n, bool on);

void gameSetLastItem(int n);

void gameSetBossRoom(bool on);

bool gameGetTimer();
void gameSetTimer(bool on);

void gameSetWarp(int x, int y, int lvl, int screen);

void gameRingBell();
void gamePlaySecret();

int gameGetSecretTimer();

int gameGetQuake();
void gameSetQuake(int n);