#include "system.hpp"
#include "callbacks.hpp"
#include "../../../source/savedata.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <fat.h>
#include <gccore.h>
#include <sys/stat.h>

#include <ogc/lwp_watchdog.h>
#include <ogc/machine/processor.h>

char const* SYS_DATAPATH 	 = "./";
char const* SYS_INIPATH  	 = "./system.ini";
char const* SYS_SAVEPATH 	 = "./map/018.map";
char const* SYS_TEMPSAVEPATH = "./data/save.tmp";

static bool quitGame = false;
static int exitCode = 0;

void resetCallback(u32 irq, void* ctx)
{
	exitCode = 2;
	quitGame = true;
}

void powerCallback(void)
{
	exitCode = 1;
	quitGame = true;
}

void powerCallback(int id)
{
	powerCallback();
}

int sys_Init()
{
	CONF_Init();
	fatInitDefault();

	SYS_SetResetCallback(resetCallback);
	SYS_SetPowerCallback(powerCallback);

	return 0;
}

void sys_Exit()
{
	if (exitCode == 1)
		SYS_ResetSystem(SYS_POWEROFF, 0, 0);

	else if (exitCode == 2)
		SYS_ResetSystem(SYS_HOTRESET, 0, 0);
}

unsigned long sys_GetMillisecond()
{
	return ticks_to_millisecs(gettime());
}

int sys_MainLoop()
{
	return !quitGame;
}

void sys_QuitGame()
{
	quitGame = true;
}

int sys_GetLanguage()
{
	int ret = LANG_JAPANESE;

	switch (CONF_GetLanguage())
	{
		case CONF_LANG_JAPANESE: ret = LANG_JAPANESE; break;
		case CONF_LANG_KOREAN:	 ret = LANG_KOREAN; break;
		case CONF_LANG_ENGLISH:  ret = LANG_ENGLISH; break;
		case CONF_LANG_GERMAN:   ret = LANG_ENGLISH; break;
		case CONF_LANG_FRENCH:   ret = LANG_ENGLISH; break;
		case CONF_LANG_SPANISH:  ret = LANG_ENGLISH; break;
		case CONF_LANG_ITALIAN:  ret = LANG_ENGLISH; break;
		case CONF_LANG_DUTCH:    ret = LANG_ENGLISH; break;
		default: ret = LANG_JAPANESE;
	}

	return ret;
}

unsigned short sys_ByteSwap16(unsigned short val)
{
	return bswap16(val);
}

unsigned int sys_ByteSwap32(unsigned int val)
{
	return bswap32(val);
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