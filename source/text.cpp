#include "text.hpp"
#include "assets.hpp"
#include "system.hpp"
#include "game.hpp"
#include <stdio.h>
#include <string.h>

#define TABLE_SIZE (50*12)
static unsigned int characterTable[TABLE_SIZE] = {
	' ', 0, 0, ',', '.', 0, ':', ';', '?', '!', 
	'"', 0, '\'', 0, 0, '^', 0, '_', 0, 0, 
	0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 
	'/', '\\', '~', 0, '|', 0xE280A6, 0, 0, 0, 0, 
	0, '(', ')', 0, 0, 0, 0, '{', '}', '<', 

	'>', 0, 0, 0, 0, 0, 0, '[', ']', '+', 
	0, 0, 0, 0, '=', 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, '$', 
	0, 0, '%', '#', '&', '*', '@', 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	0, 0, 0, '0', '1', '2', '3', '4', '5', '6', 
	'7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 
	'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, 0, 

	0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 
	'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 
	's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 
	0, 0, 0xE38181, 0xE38182, 0xE38183, 0xE38184, 0xE38185, 0xE38186, 0xE38187, 0xE38188, 
	0xE38189, 0xE3818A, 0xE3818B, 0xE3818C, 0xE3818D, 0xE3818E, 0xE3818F, 0xE38190, 0xE38191, 0xE38192, 

	0xE38193, 0xE38194, 0xE38195, 0xE38196, 0xE38197, 0xE38198, 0xE38199, 0xE3819A, 0xE3819B, 0xE3819C, 
	0xE3819D, 0xE3819E, 0xE3819F, 0xE381A0, 0xE381A1, 0xE381A2, 0xE381A3, 0xE381A4, 0xE381A5, 0xE381A6, 
	0xE381A7, 0xE381A8, 0xE381A9, 0xE381AA, 0xE381AB, 0xE381AC, 0xE381AD, 0xE381AE, 0xE381AF, 0xE381B0, 
	0xE381B1, 0xE381B2, 0xE381B3, 0xE381B4, 0xE381B5, 0xE381B6, 0xE381B7, 0xE381B8, 0xE381B9, 0xE381BA,	
	0xE381BB, 0xE381BC, 0xE381BD, 0xE381BE, 0xE381BF, 0xE38280, 0xE38281, 0xE38282, 0xE38283, 0xE38284, 

	0xE38285, 0xE38286, 0xE38287, 0xE38288, 0xE38289, 0xE3828A, 0xE3828B, 0xE3828C, 0xE3828D, 0xE3828E, 
	0xE3828F, 0xE38290, 0xE38291, 0xE38292, 0xE38293, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0xE382A1, 0xE382A2, 0xE382A3, 0xE382A4, 
	0xE382A5, 0xE382A6, 0xE382A7, 0xE382A8, 0xE382A9, 0xE382AA, 0xE382AB, 0xE382AC, 0xE382AD, 0xE382AE, 
	0xE382AF, 0xE382B0, 0xE382B1, 0xE382B2, 0xE382B3, 0xE382B4, 0xE382B5, 0xE382B6, 0xE382B7, 0xE382B8, 

	0xE382B9, 0xE382BA, 0xE382BB, 0xE382BC, 0xE382BD, 0xE382BE, 0xE382BF, 0xE38380, 0xE38381, 0xE38382, 
	0xE38383, 0xE38384, 0xE38385, 0xE38386, 0xE38387, 0xE38388, 0xE38389, 0xE3838A, 0xE3838B, 0xE3838C, 
	0xE3838D, 0xE3838E, 0xE3838F, 0xE38390, 0xE38391, 0xE38392, 0xE38393, 0xE38394, 0xE38395, 0xE38396, 
	0xE38397, 0xE38398, 0xE38399, 0xE3839A, 0xE3839B, 0xE3839C, 0xE3839D, 0xE3839E, 0xE3839F, 0xE383A0,	
	0xE383A1, 0xE383A2, 0xE383A3, 0xE383A4, 0xE383A5, 0xE383A6, 0xE383A7, 0xE383A8, 0xE383A9, 0xE383AA, 

	0xE383AB, 0xE383AC, 0xE383AD, 0xE383AE, 0xE383AF, 0xE383B0, 0xE383B1, 0xE383B2, 0xE383B3, 0xE383B4, 
	0xE383B5, 0xE383B6, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 

	0xE18480, 0xE18481, 0xE18482, 0xE18483, 0xE18484, 0xE18485, 0xE18486, 0xE18487, 0xE18488, 0xE18489,
	0xE1848A, 0xE1848B, 0xE1848C, 0xE1848D, 0xE1848E, 0xE1848F, 0xE18490, 0xE18491, 0xE18492, 0xE185A1,
	0xE185A2, 0xE185A3, 0xE185A4, 0xE185A5, 0xE185A6, 0xE185A7, 0xE185A8, 0xE185A9, 0xE185AA, 0xE185AB,
	0xE185AC, 0xE185AD, 0xE185AE, 0xE185AF, 0xE185B0, 0xE185B1, 0xE185B2, 0xE185B3, 0xE185B4, 0xE185B5,
	0xE186A8, 0xE186A9, 0xE186AA, 0xE186AB, 0xE186AC, 0xE186AD, 0xE186AE, 0xE186AF, 0xE186B0, 0xE186B1,

	0xE186B2, 0xE186B3, 0xE186B4, 0xE186B5, 0xE186B6, 0xE186B7, 0xE186B8, 0xE186B9, 0xE186BA, 0xE186BB,
	0xE186BC, 0xE186BD, 0xE186BE, 0xE186BF, 0xE186C0, 0xE186C1, 0xE186C2, 0xE186C3, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#define MAX_CHARACTERS (48*3)

typedef struct
{
	unsigned short character[MAX_CHARACTERS];
	unsigned char length;
	unsigned char parts;	//number of sprites per character
} Message;

static char label[][8] = {
	"jp",
	"en",
	"kr",
	"na"
};

static Message text[TEXT_LAST];
static bool forcePlain = true;

static void _decomposeHangul(unsigned int in, unsigned int* L, unsigned int* V, unsigned int* T)
{
	//crunch utf-32 into utf-16
	unsigned short val = ((in & 0xF0000) >> 4) | ((in & 0x3F00) >> 2) | (in & 0x00003F);

	unsigned short  SBase = 0xAC00;
	unsigned int LBase = 0xE18480;
	unsigned int VBase = 0xE185A1;
	unsigned int TBase = 0xE186A7;
	unsigned short VCount = 21;
	unsigned short TCount = 28;
	unsigned short NCount = VCount * TCount;
	unsigned short SIndex = val - SBase;
	
	*L = LBase + SIndex / NCount;
	*V = VBase + (SIndex % NCount) / TCount;
	*T = TBase + SIndex % TCount;
	if (*T == TBase) *T = 0;
}

//Get character table index from UTF-32
static unsigned short _getChar(unsigned int in)
{
	if (in == 0)
		return 0;

	for (unsigned short i = 0; i < TABLE_SIZE; i++)
	{
		if (characterTable[i] == in)
			return i;
	}

	return 0;
}

static bool _readCharacter(FILE* f, unsigned int* val)
{
	*val = 0;
	bool newline = false;

	for (int i = 0; i < 3; i++)
	{
		unsigned char c = 0;

		if (fread(&c, 1, 1, f) == 1)
		{
			//detect new line
			if (c == 0x0A)
			{
				newline = true;
			}
			else if (c == 0x0D)
			{
				fread(&c, 1, 1, f);
				newline = true;
			}
			else
			{
				*val = (*val << 8) | c;
			}

			if (c < 128)
				break;
		}
		else
		{
			newline = true;
		}
	}

	return newline;
}

static void _readText(FILE* f, Message* m)
{	
	if (gameGetLanguage() == LANG_KOREAN)
		m->parts = 3;
	else
		m->parts = 1;

	//clear
	for (int i = 0; i < MAX_CHARACTERS; i++)
		m->character[i] = 0;

	//
	int len = 0;

	for (int pos = 0; pos < MAX_CHARACTERS - m->parts; pos += m->parts)
	{	
		unsigned int val = 0;
		bool newline = _readCharacter(f, &val);

		if (newline)
		{
			break;
		}
		else
		{
			//korean characters
			if (val >= 0xEAB080 && val <= 0xED9EA3)
			{
				unsigned int L = 0;
				unsigned int V = 0;
				unsigned int T = 0;

				_decomposeHangul(val, &L, &V, &T);

				m->character[pos]   = _getChar(L);
				m->character[pos+1] = _getChar(V);
				m->character[pos+2] = _getChar(T);
			}
			else
			{
				m->character[pos] = _getChar(val);
			}
			
			len += 1;
		}
	}

	m->length = len;
}

static void _readBoldText(FILE* f, Message* m)
{
	char c = ' ';
	int pos = 0;

	while (1)
	{
		fread(&c, 1, 1, f);

		if (c == 0x0D)
		{
			fread(&c, 1, 1, f);
			c = '\n';
		}

		if (c == '\n' || c == EOF)
			break;

		m->character[pos] = c - ' ';
		pos++;
	}

	m->length = pos;
	m->parts = 1;
}

void loadText(int lang)
{
	//wrap around supported languages
	if (lang < 0) lang = LANG_END - 1;
	if (lang >= LANG_END) lang = 0;

	//
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
		//
		gameSetLanguage(lang);
		forcePlain = false;

		if (gameGetLanguage() == LANG_KOREAN)
			forcePlain = true;

		//
		for (int i = 0; i < TEXT_LAST; i++)
		{			
			//bold text
			if (i <= TEXT_THE_END && !forcePlain)
				_readBoldText(f, &(text[i]));
			//plain text
			else
				_readText(f, &(text[i]));
		}	
	}

	fclose(f);
}

int drawText(int line, float x, float y, bool center)
{
	Message* m = &text[line];

	int charW = 10;
	int charH = 16;

	if (center)
		x -= (charW-1) * m->length / 2;

	int pos = 0;
	for (int i = 0; i < m->length; i++)
	{
		for (int a = 0; a < m->parts; a++)
		{
			if (m->character[pos] != 0)
				gfx_DrawTile(images[imgFontKana], x, y, charW, charH, m->character[pos]);
			pos += 1;
		}

		x += charW-1;
	}

	return x;
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

char* getLanguageLabel(int lang)
{
	if (lang < 0 || lang >= LANG_END)
		lang = 0;

	return label[lang];
}

int drawTextBold(int line, float x, float y, char col, bool centered)
{
	Message* m = &(text[line]);
	int pos = 0;
	int charw = 8;

	if (forcePlain)
		charw = 9;

	if (centered)
		x -= charw * (m->length / 2.f);

	for (int i = 0; i < m->length; i++)
	{
		for (int a = 0; a < m->parts; a++)
		{
			if (m->character[pos] != 0)
			{
				if (forcePlain)
					gfx_DrawTile(images[imgFontKana], x, y-8, 10, 16, m->character[pos]);
				else
					gfx_DrawTile(images[imgFont], x, y, 8, 8, m->character[pos] + (col * 64));
			}

			pos += 1;
		}

		x += charw;
	}

	return x;
}

int drawTextBold(char const* str, float x, float y, char col, bool centered)
{
	if (forcePlain)
	{
		//Plain
		int c = 0;
		int i = 0;

		if (centered)
			x -= 9.f * (strlen(str) / 2.f);

		while (str[i] != '\0')
		{
			c = _getChar(str[i]);

			if (c != 0)
				gfx_DrawTile(images[imgFontKana], x, y-8, 10, 16, c);

			x += 9;
			i += 1;
		}

		return x;
	}

	return drawBold(str, x, y, col, centered);
}

int drawBold(char const* str, float x, float y, char col, bool centered)
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

		if (c != 0)
			gfx_DrawTile(images[imgFont], x, y, 8, 8, c + (col * 64));

		x += 8;
		i+=1;
	}

	return x;
}