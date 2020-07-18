#pragma once

extern char const* SYS_DATAPATH;
extern char const* SYS_INIPATH;
extern char const* SYS_SAVEPATH;
extern char const* SYS_TEMPSAVEPATH;

enum {
	LANG_JAPANESE = 0,
	LANG_ENGLISH = 1,
	LANG_END
};

int sys_Init();
void sys_Exit();

int sys_MainLoop();
void sys_QuitGame();

#ifndef _NO_FRAMESKIP
unsigned long sys_GetMillisecond();
#endif

int sys_GetLanguage();

//Byte swapping, only used for big-endian machines.
unsigned short sys_ByteSwap16(unsigned short val);
unsigned int sys_ByteSwap32(unsigned int val);

bool sys_SaveFileExists(char const* fname);
bool sys_DeleteSaveFile(char const* fname);
bool sys_ReadSaveFile(char const* fname, void* data);
bool sys_WriteSaveFile(char const* fname, void* data);