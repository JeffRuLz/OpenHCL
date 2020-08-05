/*
*	Ported and translated from Quadruple D's QDArc.pas file.
*	http://karen.saiin.net/~hayase/dddd/download.html
*/

//Archive unit

#include "QDArc.hpp"
#include "system.hpp"
#include <string.h>
#include <ctype.h>
#include <malloc.h>

//QDA Format...
/*

 OFS  |   SIZE   |  TYPE         | NAME      |   COMMENT
 ------------------------------------------------------------------
  +0    4           Char * 4       Signature    'QDA0'
  +4    4           DWORD          Compress?    0:No Comp.  1:Compressed
  +8    4           DWORD          DataCount    Number of Data
  +12   244         BYTE           Reserved     Reserved

  +256  268*nData   QDADataHeader  dataHeader   informations of each data
  ???   ??? *nData  ???            ---          DataBlock
*/

#pragma pack(push, 1)
typedef struct 
{
	unsigned int Compress; 	//Are you compressing? 0=No 1=Yes
	char Signature[4];		//Signature 'QDA0'
	unsigned int DataCount;	//The number of data
	char Reserved[244];
} TQDAFileHeader;

typedef struct 
{
	unsigned int Offset; 			//Offset from the "beginning" of the file
	unsigned int Length;			//Data length (after storage)
	unsigned int RestoredLength;	//Data length (after decompression)
	char ID[256];					//ID
} TQDADataHeader;
#pragma pack(pop)


//QDAFileHeader check
static bool CheckFileHeader(TQDAFileHeader& qfh)
{
	return 
		(qfh.Signature[0] == 'Q') and
		(qfh.Signature[1] == 'D') and
		(qfh.Signature[2] == 'A') and
		(qfh.Signature[3] == '0');

}

static void AnsiLowerCase(char const* in, char* out)
{
	unsigned int i = 0;
	for (i = 0; i < strlen(in); i++)
	{
		out[i] = tolower(in[i]);
	}

	out[i] = '\0';
}

FILE* ExtractFromQDAStream(FILE* Source, char const* ID)
{
	unsigned int i;
	TQDAFileHeader qfh;
	TQDADataHeader qdh;
	TQDADataHeader* qdhs = nullptr;

	//First, read the header
	fread(&qfh, sizeof(qfh), 1, Source);
	
	qfh.Compress = sys_ByteSwap32(qfh.Compress);
	qfh.DataCount = sys_ByteSwap32(qfh.DataCount);

	if (not CheckFileHeader(qfh))
	{
		#ifdef _DEBUG
			printf("Quadruple D Archive is not data that can be handled.\n");
		#endif
		goto error;
	}

	//Reading the data header
	qdhs = (TQDADataHeader*)malloc(sizeof(qdh) * qfh.DataCount);		
	fread(qdhs, sizeof(qdh) * qfh.DataCount, 1, Source);

	//ID search
	char IDlower[256];
	AnsiLowerCase(ID, IDlower);

	fseek(Source, 0, SEEK_SET);
	for (i = 0; i < qfh.DataCount; i++)
	{
		AnsiLowerCase(qdhs[i].ID, qdhs[i].ID);
		if (strcmp(qdhs[i].ID, IDlower) == 0)
		{
			qdhs[i].Offset = sys_ByteSwap32(qdhs[i].Offset);
			qdhs[i].Length = sys_ByteSwap32(qdhs[i].Length);
			qdhs[i].RestoredLength = sys_ByteSwap32(qdhs[i].RestoredLength);
			
			qdh = qdhs[i];
			fseek(Source, qdhs[i].Offset, SEEK_SET);
			break;
		}
	}

	//Can not find
	if (ftell(Source) == 0)
	{
		#ifdef _DEBUG
			printf("Data with the specified ID '%s' in the archive can not be found.\n", ID);
		#endif
		fclose(Source);
		Source = nullptr;
		goto error;
	}

	//Reading data
	if (qfh.Compress == 1)
	{
		//Ignoring compression
		#ifdef _DEBUG
			printf("Compression is not supported.\n");
		#endif
		fclose(Source);
		Source = nullptr;
		goto error;
	}

error: //...or cleanup
	if (qdhs != nullptr)
	{
		free(qdhs);
		qdhs = nullptr;
	}

	return Source;
}

FILE* ExtractFromQDAFile(char const* FileName, char const* ID)
{
	FILE* fs = fopen(FileName, "rb");

	if (fs)
		return ExtractFromQDAStream(fs, ID);

	fclose(fs);
	return nullptr;
}