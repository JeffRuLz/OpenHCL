#pragma once

enum {
	TEXT_COLOR_WHITE  = 0,
	TEXT_COLOR_RED 	  = 1,
	TEXT_COLOR_YELLOW = 2
};

void loadText(int lang);
int drawText(int line, float x, float y, bool center = false);
int drawChar(char c, float x, float y);
int messageLength(int line);

char* getLanguageLabel(int lang);

int drawTextBold(int line, float x, float y, char col = TEXT_COLOR_WHITE, bool centered = false);
int drawTextBold(char const* str, float x, float y, char col = TEXT_COLOR_WHITE, bool centered = false);
int drawBold(char const* str, float x, float y, char col = TEXT_COLOR_WHITE, bool centered = false);

enum {
	TEXT_NEW_GAME 	= 0,
	TEXT_LOAD_GAME 	= 1,
	TEXT_EXIT 		= 2,
	TEXT_GAME_OVER	= 3,
	TEXT_CONTINUE 	= 4,
	TEXT_RESET 		= 5,
	TEXT_OPTIONS 	= 6,
	TEXT_CTRL_RESET = 7,
	TEXT_ESC_EXIT 	= 8,
	TEXT_ON 		= 9,
	TEXT_OFF 		= 10,
	TEXT_LANGUAGE 	= 11,
	TEXT_MUSIC 		= 12,
	TEXT_SOUND 		= 13,
	TEXT_TIMER		= 14,
	TEXT_VIDEO		= 15,
	TEXT_FULLSCREEN	= 16,
	TEXT_VSYNC		= 17,
	TEXT_BACK		= 18,
	TEXT_SUB_WEAPON	= 19,
	TEXT_ITEM 		= 20,
	TEXT_KEY 		= 21,
	TEXT_ALL_CLEAR	= 22,
	TEXT_TIME 		= 23,
	TEXT_TREASURE 	= 24,
	TEXT_STAFF 		= 25,
	TEXT_SPRITES 	= 26,
	TEXT_PROGRAM 	= 27,
	TEXT_TEST_PLAYER = 29,
	TEXT_SPECIAL_THANKS = 29,
	TEXT_SOUND_EFFECTS = 30,
	TEXT_ENGLISH_TRANSLATION = 31,
	TEXT_KOREAN_TRANSLATION = 32,
	TEXT_LANG3		= 33, //placeholders for future languages
	TEXT_LANG4		= 34,
	TEXT_LANG5		= 35,
	TEXT_LANG6 		= 36,
	TEXT_LANG7		= 37,
	TEXT_LANG8		= 38,
	TEXT_LANG9		= 39,
	TEXT_LANG10		= 40,
	TEXT_THE_END	= 41,
	TEXT_SAVING 	= 42,
	TEXT_SAVE_ERROR = 43,
	TEXT_DUNGEON 	= 46,
	TEXT_ITEM_NAMES = 54,
	TEXT_FOUND 		= 96,
	TEXT_ITEM_DESC 	= 97,
	TEXT_LAST 		= 125		
};