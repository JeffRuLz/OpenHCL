#include "system.hpp"
#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../../../source/savedata.hpp"

char const* SYS_DATAPATH 	 = "romfs:/";
char const* SYS_INIPATH  	 = "sdmc:/3ds/OpenHCL/system.ini";
char const* SYS_SAVEPATH 	 = "sdmc:/3ds/OpenHCL/map/018.map";
char const* SYS_TEMPSAVEPATH = "sdmc:/3ds/OpenHCL/data/save.tmp";

static bool quitGame = false;

int sys_Init()
{
	osSetSpeedupEnable(true);

	if (cfguInit())
	{
		#ifdef _DEBUG
		printf("cfguInit() failure.\n");
		#endif
	}

	if (romfsInit())
	{
		#ifdef _DEBUG
		printf("romfsInit() failure.\n");
		#endif
	}

	//create save directories
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/OpenHCL", 0777);
	#ifndef _NO_TEMP_SAVE
	mkdir("sdmc:/3ds/OpenHCL/data", 0777);
	#endif
	mkdir("sdmc:/3ds/OpenHCL/map", 0777);

	return 0;
}

void sys_Exit()
{
	romfsExit();
	cfguExit();
}
/*
unsigned long sys_GetMillisecond()
{
	return osGetTime();
}
*/
int sys_MainLoop()
{
	bool ret = aptMainLoop();

	if (quitGame)
		return false;

	return ret;
}

void sys_QuitGame()
{
	quitGame = true;
}

int sys_GetLanguage()
{
	int ret = LANG_JAPANESE;

	u8 l;
	CFGU_GetSystemLanguage(&l);

	switch (l)
	{
		case CFG_LANGUAGE_JP: ret = LANG_JAPANESE; break;
		case CFG_LANGUAGE_EN: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_FR: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_DE: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_IT: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_ES: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_NL: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_PT: ret = LANG_ENGLISH; break;
		case CFG_LANGUAGE_RU: ret = LANG_ENGLISH; break;
		default: ret = LANG_JAPANESE;
	}

	return ret;
}

unsigned short sys_ByteSwap16(unsigned short val)
{
	return val;
}

unsigned int sys_ByteSwap32(unsigned int val)
{
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
		fread(ptr, sizeof(SaveData), 1, f);
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
		fwrite(ptr, sizeof(SaveData), 1, f);
	}

	fclose(f);
	return true;
}