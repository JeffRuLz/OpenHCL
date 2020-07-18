#include "text.hpp"
#include "assets.hpp"
#include "system.hpp"
#include "game.hpp"
#include <stdio.h>
#include <string.h>

typedef struct
{
	unsigned int character[48];
	char length;
} Message;

static char label[][8] = {
	"jp",
	"en",
	"na"
};

static char plainText[TEXT_THE_END+1][32];
static Message text[83];

#define TABLE_SIZE 480
static unsigned long characterTable[TABLE_SIZE] = {
	' ', 0, 0, ',', '.', 0, ':', ';', '?', '!', '"', 0, '\'', 0, 0, '^', 0, '_', 0, 0,
	0, 0, 0, 0, 0, 0, 0, '-', 0, 0, '/', '\\', '~', 0, '|', 0xE280A6, 0, 0, 0, 0,
	0, '(', ')', 0, 0, 0, 0, '{', '}', '<', '>', 0, 0, 0, 0, 0, 0, '[', ']', '+',
	0, 0, 0, 0, '=', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '$',
	0, 0, '%', '#', '&', '*', '@', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0,
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0,
	0, 0, 0xE38181, 0xE38182, 0xE38183, 0xE38184, 0xE38185, 0xE38186, 0xE38187, 0xE38188, 0xE38189, 0xE3818A, 0xE3818B, 0xE3818C, 0xE3818D, 0xE3818E, 0xE3818F, 0xE38190, 0xE38191, 0xE38192,
	0xE38193, 0xE38194, 0xE38195, 0xE38196, 0xE38197, 0xE38198, 0xE38199, 0xE3819A, 0xE3819B, 0xE3819C, 0xE3819D, 0xE3819E, 0xE3819F, 0xE381A0, 0xE381A1, 0xE381A2, 0xE381A3, 0xE381A4, 0xE381A5, 0xE381A6,
	0xE381A7, 0xE381A8, 0xE381A9, 0xE381AA, 0xE381AB, 0xE381AC, 0xE381AD, 0xE381AE, 0xE381AF, 0xE381B0, 0xE381B1, 0xE381B2, 0xE381B3, 0xE381B4, 0xE381B5, 0xE381B6, 0xE381B7, 0xE381B8, 0xE381B9, 0xE381BA,
	0xE381BB, 0xE381BC, 0xE381BD, 0xE381BE, 0xE381BF, 0xE38280, 0xE38281, 0xE38282, 0xE38283, 0xE38284, 0xE38285, 0xE38286, 0xE38287, 0xE38288, 0xE38289, 0xE3828A, 0xE3828B, 0xE3828C, 0xE3828D, 0xE3828E,
	0xE3828F, 0xE38290, 0xE38291, 0xE38292, 0xE38293, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xE382A1, 0xE382A2, 0xE382A3, 0xE382A4,
	0xE382A5, 0xE382A6, 0xE382A7, 0xE382A8, 0xE382A9, 0xE382AA, 0xE382AB, 0xE382AC, 0xE382AD, 0xE382AE, 0xE382AF, 0xE382B0, 0xE382B1, 0xE382B2, 0xE382B3, 0xE382B4, 0xE382B5, 0xE382B6, 0xE382B7, 0xE382B8,
	0xE382B9, 0xE382BA, 0xE382BB, 0xE382BC, 0xE382BD, 0xE382BE, 0xE382BF, 0xE38380, 0xE38381, 0xE38382, 0xE38383, 0xE38384, 0xE38385, 0xE38386, 0xE38387, 0xE38388, 0xE38389, 0xE3838A, 0xE3838B, 0xE3838C,
	0xE3838D, 0xE3838E, 0xE3838F, 0xE38390, 0xE38391, 0xE38392, 0xE38393, 0xE38394, 0xE38395, 0xE38396, 0xE38397, 0xE38398, 0xE38399, 0xE3839A, 0xE3839B, 0xE3839C, 0xE3839D, 0xE3839E, 0xE3839F, 0xE383A0,
	0xE383A1, 0xE383A2, 0xE383A3, 0xE383A4, 0xE383A5, 0xE383A6, 0xE383A7, 0xE383A8, 0xE383A9, 0xE383AA, 0xE383AB, 0xE383AC, 0xE383AD, 0xE383AE, 0xE383AF, 0xE383B0, 0xE383B1, 0xE383B2, 0xE383B3, 0xE383B4,
	0xE383B5, 0xE383B6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//Convert utf-8 to font map
static unsigned int _getChar(unsigned long in)
{
	for (unsigned int i = 0; i < TABLE_SIZE; i++)
	{
		if (in == characterTable[i])
			return i;
	}

	return 0;
}

static void _readText(FILE* f, Message* m)
{
	bool newline = false;

	//read each character
	int pos = 0;
	for (pos = 0; pos < 48; pos++)
	{
		//read one character
		unsigned long val = 0;

		for (int a = 0; a < 3; a++)
		{
			unsigned char in = 0;
			if (fread(&in, 1, 1, f) == 1)
			{
				//detect new line
				if (in == 0x0D)
				{
					fread(&in, 1, 1, f);
					newline = true;
				}
				else
				{
					val = (val << 8) | in;
				}

				if (in < 128)
					break;
			}
			else
			{
				//end of file / read error
				newline = true;
			}
		}
		
		if (!newline)
			m->character[pos] = _getChar(val);
		else
			break;
	}

	m->length = pos;
}

void _readPlainText(FILE* f, char* out)
{
	char c = ' ';

	while (1)
	{
		fread(&c, 1, 1, f);

		if (c == 0x0D)
		{
			fread(&c, 1, 1, f);
			c = '\n';
		}

		if (c == '\n' || c == EOF)
		{
			*out = '\0';
			break;
		}

		*out = c;
		out++;
	}
}

void loadText(int lang)
{
	if (lang < 0) { lang = LANG_END - 1; }
	if (lang >= LANG_END) { lang = 0; }

	char fpath[128];
	sprintf(fpath, "%sdata/%s.txt", SYS_DATAPATH, label[lang]);

	FILE* f = fopen(fpath, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("Error opening text file %s\n", fpath);
		#endif
	}
	else
	{
		gameSetLanguage(lang);

		for (int i = 0; i <= TEXT_THE_END; i++)
		{
			_readPlainText(f, plainText[i]);

			#ifdef _DEBUG
			printf("%s\n", plainText[i]);
			#endif
		}

		for (int i = 0; i < 83; i++)
			_readText(f, &(text[i]));

		fclose(f);
	}
}

int drawText(int line, float x, float y, bool center)
{
	Message* m = &text[line];

	int charW = 10;
	int charH = 16;

	if (center)
		x -= (charW-1) * m->length / 2;

	for (int i = 0; i < m->length; i++)
		gfx_DrawTile(images[imgFontKana], x + ((charW-1) * i), y, charW, charH, m->character[i]);

	return x + (charW-1) * m->length;
}

int drawChar(char c, float x, float y)
{
	int charW = 10;
	int charH = 16;

	gfx_DrawTile(images[imgFontKana], x, y, charW, charH, _getChar(c));

	return x + charW - 1;
}

int messageLength(int line)
{
	Message* m = &text[line];

	return m->length;
}

char* getText(int line)
{
	return plainText[line];
}

char* getLanguageLabel(int lang)
{
	if (lang < 0 || lang >= LANG_END)
		lang = 0;

	return label[lang];
}

int drawTextBold(int line, float x, float y, char col, bool centered)
{
	return drawTextBold(plainText[line], x, y, col, centered);
}

int drawTextBold(char const* str, float x, float y, char col, bool centered)
{
	char c = ' ';
	int i = 0;

	if (centered)
		x -= 8.f * (strlen(str) / 2.f);

	while (str[i] != '\0')
	{
		c = str[i];

		//to uppercase
		if (c >= 'a' && c <= 'z')
			c -= 'a' - 'A';

		c -= ' ';

		gfx_DrawTile(images[imgFont], x, y, 8, 8, c + (col * 64));

		x += 8;
		i+=1;
	}

	return x;
}