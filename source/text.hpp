#pragma once

//plain text
enum {
	TEXT_NEW_GAME,
	TEXT_LOAD_GAME,
	TEXT_EXIT,
	TEXT_CONTINUE,
	TEXT_RESET,
	TEXT_OPTIONS,
	TEXT_CTRL_RESET,
	TEXT_ESC_EXIT,
	TEXT_ON,
	TEXT_OFF,
	TEXT_LANGUAGE,
	TEXT_MUSIC,
	TEXT_SOUND,
	TEXT_TIMER,
	TEXT_VIDEO,
	TEXT_FULLSCREEN,
	TEXT_VSYNC,
	TEXT_BACK,
	TEXT_SUB_WEAPON,
	TEXT_ITEM,
	TEXT_KEY,
	TEXT_ALL_CLEAR,
	TEXT_TIME,
	TEXT_TREASURE,
	TEXT_STAFF,
	TEXT_SPRITES,
	TEXT_PROGRAM,
	TEXT_TEST_PLAYER,
	TEXT_SPECIAL_THANKS,
	TEXT_SOUND_EFFECTS,
	TEXT_ENGLISH_TRANSLATION,
	TEXT_LANG2,	//placeholders for future languages
	TEXT_LANG3,
	TEXT_LANG4,
	TEXT_LANG5,
	TEXT_LANG6,
	TEXT_LANG7,
	TEXT_LANG8,
	TEXT_LANG9,
	TEXT_LANG10,
	TEXT_THE_END
};

//encoded text
enum {
	TEXT_SAVING 	= 0,
	TEXT_SAVE_ERROR = 1,
	TEXT_DUNGEON 	= 4,
	TEXT_ITEM_NAMES = 12,
	TEXT_FOUND 		= 54,
	TEXT_ITEM_DESC 	= 55
};

enum {
	TEXT_COLOR_WHITE  = 0,
	TEXT_COLOR_RED 	  = 1,
	TEXT_COLOR_YELLOW = 2
};

void loadText(int lang);
int drawText(int line, float x, float y, bool center = false);
int drawChar(char c, float x, float y);
int messageLength(int line);

char* getText(int line);
char* getLanguageLabel(int lang);

int drawTextBold(int line, float x, float y, char col = TEXT_COLOR_WHITE, bool centered = false);
int drawTextBold(char const* str, float x, float y, char col = TEXT_COLOR_WHITE, bool centered = false);