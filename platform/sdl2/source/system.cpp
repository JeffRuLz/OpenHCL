#include "system.hpp"
#include "graphics.hpp"
#include <SDL2/SDL.h>
#include <stdio.h>
#include "../../../source/game.hpp"
#include "../../../source/savedata.hpp"
#ifdef _VITA
#include <psp2/apputil.h>
#include <psp2/system_param.h>
#include <psp2/io/stat.h> 
#endif

#ifdef _VITA
char const* SYS_DATAPATH 	 = "app0:/data/";
char const* SYS_INIPATH  	 = "ux0:/data/OpenHCL/system.ini";
char const* SYS_SAVEPATH 	 = "ux0:/data/OpenHCL/map/018.map";
char const* SYS_TEMPSAVEPATH = "ux0:/data/OpenHCL/data/save.tmp";
#else
char const* SYS_DATAPATH 	 = "./";
char const* SYS_INIPATH  	 = "./system.ini";
char const* SYS_SAVEPATH 	 = "./map/018.map";
char const* SYS_TEMPSAVEPATH = "./data/save.tmp";
#endif

static SDL_Event e;
static bool gameIsRunning = true;

static bool ctrlDown = false;
static bool altDown = false;

int sys_Init()
{
#ifdef _VITA
	SceAppUtilInitParam init;
	SceAppUtilBootParam boot;
	memset(&init, 0, sizeof(SceAppUtilInitParam));
	memset(&boot, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init, &boot);
	
	sceIoMkdir("ux0:/data/", 0777);
	sceIoMkdir("ux0:/data/OpenHCL/", 0777);
	sceIoMkdir("ux0:/data/OpenHCL/map/", 0777);
	sceIoMkdir("ux0:/data/OpenHCL/data/", 0777);
#endif
	
	int ret = SDL_Init(0);

	if (ret < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return ret;
	}

	return 0;
}

void sys_Exit()
{
	SDL_Quit();
}

unsigned long sys_GetMillisecond()
{
	return SDL_GetTicks();
}

int sys_MainLoop()
{
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type ==  SDL_QUIT)
		{
			sys_QuitGame();
		}
		else if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
				case SDLK_RALT:
				case SDLK_LALT:
					altDown = true;
					break;

				case SDLK_RCTRL:
				case SDLK_LCTRL:
					ctrlDown = true;
					break;

				case SDLK_ESCAPE:
				{
					GameState s = gameGetState();
					if (s == GAME_STATE_OPTIONS || s == GAME_STATE_TITLESCREEN || s == GAME_STATE_CREDITS)
						sys_QuitGame();
				}
				break;
#ifdef _HAS_FULLSCREEN
				case SDLK_RETURN:
				{
					if (altDown)
						gfx_SetFullscreen(!gfx_GetFullscreen());
				}
				break;
#endif
				case SDLK_r:
				{
					if (ctrlDown)
						gameReset();
				}
				break;
			}
		}
		else if (e.type == SDL_KEYUP)
		{
			switch (e.key.keysym.sym)
			{
				case SDLK_RALT:
				case SDLK_LALT:
					altDown = false;
					break;

				case SDLK_RCTRL:
				case SDLK_LCTRL:
					ctrlDown = false;
					break;
			}
		}
	}

	return gameIsRunning;
}

void sys_QuitGame()
{
	gameIsRunning = false;
}

int sys_GetLanguage()
{
#ifdef _VITA
	int ret = LANG_JAPANESE;
	int l = 0;
	
	if (sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, &l) == 0)
	{
		switch (l)
		{
			case SCE_SYSTEM_PARAM_LANG_JAPANESE:   	  ret = LANG_JAPANESE; break;
			case SCE_SYSTEM_PARAM_LANG_KOREAN:	  	  ret = LANG_KOREAN; break;
			case SCE_SYSTEM_PARAM_LANG_ENGLISH_US: 	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_ENGLISH_GB: 	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_FRENCH: 	  	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_SPANISH: 	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_GERMAN: 	  	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_ITALIAN: 	  ret = LANG_ENGLISH; break;
			case SCE_SYSTEM_PARAM_LANG_DUTCH: 	  	  ret = LANG_ENGLISH; break;
			default: ret = LANG_JAPANESE;
		}
	}

	return ret;
#else
	return LANG_JAPANESE;
#endif
}

unsigned short sys_ByteSwap16(unsigned short val)
{
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		val = SDL_Swap16(val);

	return val;
}

unsigned int sys_ByteSwap32(unsigned int val)
{
	if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		val = SDL_Swap32(val);

	return val;
}

bool sys_SaveFileExists(char const* fname)
{
	FILE* f = fopen(fname, "rb");

	if (!f)
		return false;

	fclose(f);
	return true;
}

bool sys_DeleteSaveFile(char const* fname)
{
	return (remove(fname) == 0);
}

bool sys_ReadSaveFile(char const* fname, void* data)
{
	FILE* f = fopen(fname, "rb");
	if (!f)
	{
		return false;
	}
	else
	{
		SaveData* ptr = (SaveData*)data;

		fread(&ptr->health, 4, 1, f);
		fread(&ptr->maxHealth, 4, 1, f);
		fread(&ptr->ammo, 4, 1, f);
		fread(&ptr->maxAmmo, 4, 1, f);
		fread(&ptr->weapon, 4, 1, f);

		for (int i = 0; i < 4500; i++)
			fread(&ptr->flags[i], 1, 1, f);

		fread(&ptr->unknown, 4, 1, f);
		fread(&ptr->playTime, 4, 1, f);
		fread(&ptr->heroX, 4, 1, f);
		fread(&ptr->heroY, 4, 1, f);
		fread(&ptr->level, 4, 1, f);
		fread(&ptr->screen, 4, 1, f);
		fread(&ptr->heroDirection, 4, 1, f);

		ptr->health    	   = sys_ByteSwap32(ptr->health);
		ptr->maxHealth 	   = sys_ByteSwap32(ptr->maxHealth);
		ptr->ammo 	   	   = sys_ByteSwap32(ptr->ammo);
		ptr->maxAmmo   	   = sys_ByteSwap32(ptr->maxAmmo);
		ptr->weapon    	   = sys_ByteSwap32(ptr->weapon);
		ptr->playTime  	   = sys_ByteSwap32(ptr->playTime);
		ptr->heroX 	   	   = sys_ByteSwap32(ptr->heroX);
		ptr->heroY 	   	   = sys_ByteSwap32(ptr->heroY);
		ptr->level 	   	   = sys_ByteSwap32(ptr->level);
		ptr->screen    	   = sys_ByteSwap32(ptr->screen);
		ptr->heroDirection = sys_ByteSwap32(ptr->heroDirection);
	}

	fclose(f);
	return true;
}

bool sys_WriteSaveFile(char const* fname, void* data)
{
	FILE* f = fopen(fname, "wb");
	if (!f)
	{
		return false;
	}
	else
	{
		SaveData* ptr = (SaveData*)data;

		unsigned long n = 0;	
		n = sys_ByteSwap32(ptr->health); 	fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->maxHealth); fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->ammo); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->maxAmmo); 	fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->weapon); 	fwrite(&n, sizeof(unsigned long), 1, f);

		for (int i = 0; i < 4500; i++)
			fwrite(&ptr->flags[i], 1, 1, f);

		n = sys_ByteSwap32(ptr->unknown); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->playTime); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->heroX); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->heroY); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->level); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->screen); 		fwrite(&n, sizeof(unsigned long), 1, f);
		n = sys_ByteSwap32(ptr->heroDirection); fwrite(&n, sizeof(unsigned long), 1, f);
	}

	fclose(f);
	return true;
}