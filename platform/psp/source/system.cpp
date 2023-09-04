#include "system.hpp"
#include <stdio.h>
#include <cstdlib>
#include "../../../source/savedata.hpp"

#include <pspkernel.h>
#include <psputility_sysparam.h>
#include <psppower.h>
#include "callbacks.h"

PSP_MODULE_INFO("OpenHCL", 0, 1, 1); // Define the module info section
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU); // Define the main thread's attribute value (optional)
PSP_HEAP_SIZE_KB(-1024);

char const* SYS_DATAPATH 	 = "./";
char const* SYS_INIPATH  	 = "./system.ini";
char const* SYS_SAVEPATH 	 = "./map/018.map";
char const* SYS_TEMPSAVEPATH = "./data/save.tmp";

int sys_Init()
{
	setupCallbacks();

//	if (scePowerGetCpuClockFrequency() < 266)
//		scePowerSetClockFrequency(266,266,133);

	return 0;
}

void sys_Exit()
{
	sceKernelExitGame();
}
/*
unsigned long sys_GetMillisecond()
{
	return sceKernelLibcClock() / 1000;
}
*/
int sys_MainLoop()
{
	return running();
}

void sys_QuitGame()
{
	exitCallback(0,0,NULL);
}

int sys_GetLanguage()
{
	int ret = LANG_JAPANESE;

	int l;
	if (sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &l) == 0)
	{
		switch (l)
		{
			case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:   ret = LANG_JAPANESE; break;
			case PSP_SYSTEMPARAM_LANGUAGE_KOREAN:	  ret = LANG_KOREAN; break;
			case PSP_SYSTEMPARAM_LANGUAGE_ENGLISH: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_FRENCH: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_SPANISH: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_GERMAN: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_ITALIAN: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_DUTCH: 	  ret = LANG_ENGLISH; break;
			case PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE: ret = LANG_ENGLISH; break;
			default: ret = LANG_JAPANESE;
		}
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