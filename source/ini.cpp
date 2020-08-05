#include "ini.hpp"
#include "system.hpp"
#include "game.hpp"
#include "audio.hpp"
#include "text.hpp"
#include "graphics.hpp"
#include <stdio.h>
#include <string.h>

static bool _readLine(FILE* f, char* out)
{
	bool ret = true;
	char c = ' ';
	
	while (c != '\0' && c != '\n' && c != EOF)
	{
		if (fread(&c, 1, 1, f) != 1)
		{
			ret = false;
			break;
		}

		//skip spaces
		if (c == ' ')
			continue;

		//to lower case
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';

		*out++ = c;
	}

	*out = '\0';

	return ret;
}

static bool _checkBool(char* str, bool def)
{
	if (strstr(str, "=on") != 0)
		return true;

	if (strstr(str, "=off") != 0)
		return false;

	return def;
}

static int _checkInt(char* str, int def)
{
	char* ptr = strrchr(str, '=');
	int num = 0;

	if (ptr != nullptr)
	{
		ptr += 1;

		while (*ptr != '\0' && *ptr != '\n')
		{
			//invalid characters
			if (*ptr < '0' || *ptr > '9')
				return def;

			num *= 10;
			num += *ptr - '0';

			ptr += 1;
		}

		return num;
	}

	return def;
}

bool iniSave()
{
	FILE* f = fopen(SYS_INIPATH, "wb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("iniSave() - could not open %s\n", SYS_INIPATH);
		#endif
		return false;
	}
	else
	{
		fprintf(f, "[disp]\n");
		
		#ifdef _HAS_FULLSCREEN
 		  fprintf(f, "fullscreen=%s\n", (gfx_GetFullscreen())? "on": "off");
		#endif
		
 		#ifdef _HAS_SIZE
		  fprintf(f, "size=%d\n", gfx_GetSize());
		#endif

		#ifdef _HAS_VSYNC
		  fprintf(f, "vsync=%s\n", (gfx_GetVsync())? "on": "off");
		#endif

		#ifdef _HAS_VIDEOMODE
		  fprintf(f, "mode=%d\n", gfx_GetVideoMode());
		#endif

		fprintf(f, "[device]\n");

		fprintf(f, "[system]\n");
		//language
		{
			fprintf(f, "language=");
			fprintf(f, "%s\n", getLanguageLabel(gameGetLanguage()));
		}
		fprintf(f, "muvol=%d\n", aud_GetMusicVolume());
		fprintf(f, "sevol=%d\n", aud_GetSoundVolume());
		fprintf(f, "timer=%s\n", (gameGetTimer())? "on": "off");
	}

	fclose(f);

	return true;
}

bool iniLoad()
{
	FILE* f = fopen(SYS_INIPATH, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("iniLoad() - Could not open %s\n", SYS_INIPATH);
		#endif
		return false;
	}
	else
	{
		char str[128] = "";

		while (_readLine(f, str))
		{
			//skip labels
			if (str[0] == '[')
				continue;

			if (strstr(str, "language=") != 0)
			{
				for (int lang = 0; lang < LANG_END; lang+=1)
				{
					char checkStr[8];
					sprintf(checkStr, "=%s", getLanguageLabel(lang));
					if (strstr(str, checkStr) != 0)
					{
						//loadText(lang);
						gameSetLanguage(lang);
						break;
					}
				}
			}

			else if (strstr(str, "muvol=") != 0)
				aud_SetMusicVolume(_checkInt(str, aud_GetMusicVolume()));

			else if (strstr(str, "sevol=") != 0)
				aud_SetSoundVolume(_checkInt(str, aud_GetSoundVolume()));

			else if (strstr(str, "timer=") != 0)
				gameSetTimer(_checkBool(str, gameGetTimer()));

			#ifdef _HAS_FULLSCREEN
				else if (strstr(str, "fullscreen=") != 0)
					gfx_SetFullscreen(_checkBool(str, gfx_GetFullscreen()));
			#endif

			#ifdef _HAS_VSYNC
				else if (strstr(str, "vsync=") != 0)
					gfx_SetVsync(_checkBool(str, gfx_GetVsync()));
			#endif

			#ifdef _HAS_VIDEOMODE
				else if (strstr(str, "mode=") != 0)
			  		gfx_SetVideoMode(_checkInt(str, gfx_GetVideoMode()));
			#endif
		}
	}

	fclose(f);

	return true;
}