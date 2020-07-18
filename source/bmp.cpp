#include "bmp.hpp"
#include "system.hpp"
#include <malloc.h>

#pragma pack(push, 1)
typedef struct 
{
	unsigned char bfType[2];
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

typedef struct
{
	unsigned int biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;
#pragma pack(pop)

Bitmap* loadBitmap(FILE* f)
{
	Bitmap* bmp = nullptr;

	if (!f)
	{
		printf("Bitmap file pointer is null\n");
	}
	else
	{
		//read file header
		BitmapFileHeader bfh;
		fread(&bfh, sizeof(BitmapFileHeader), 1, f);

		//check if signature is correct
		if (bfh.bfType[0] != 'B' || bfh.bfType[1] != 'M')
		{
			printf("File signature is incorrect.\n");
		}
		else
		{
			//read bitmap header
			BitmapInfoHeader bih;
			fread(&bih, sizeof(BitmapInfoHeader), 1, f);

			bih.biWidth = sys_ByteSwap32(bih.biWidth);
			bih.biHeight = sys_ByteSwap32(bih.biHeight);
			bih.biBitCount = sys_ByteSwap16(bih.biBitCount);

			//
			bmp = (Bitmap*)malloc(sizeof(Bitmap));
			
			bmp->width = bih.biWidth;
			bmp->height = bih.biHeight;
			bmp->bitCount = bih.biBitCount;
			bmp->palette = nullptr;
			bmp->data = nullptr;

			if (bih.biBitCount == 8)
			{
				//read palette
				int size = 256*4*sizeof(unsigned char);
				bmp->palette = (unsigned char*)malloc(size);
				fread(bmp->palette, size, 1, f);

				//allocate data
				size = bih.biWidth*bih.biHeight*sizeof(unsigned char);
				bmp->data = (unsigned char*)malloc(size);

				//flip image
				int linesize = bih.biWidth*sizeof(unsigned char);
				unsigned char* ptr = bmp->data + (linesize * (bih.biHeight-1));

				for (int y = bih.biHeight; y > 0; y -= 1)
				{
					fread(ptr, linesize, 1, f);
					ptr -= linesize;
				}
			}
			else if (bih.biBitCount == 24)
			{
				int size = bih.biWidth*bih.biHeight*3;
				bmp->data = (unsigned char*)malloc(size);
				fread(bmp->data, size, 1, f);
			}
			else
			{
				printf("Bit count is not supported: %d\n", bih.biBitCount);

				freeBitmap(bmp);
				bmp = nullptr;
			}
		}
	}

	return bmp;
}

void freeBitmap(Bitmap* bmp)
{
	if (!bmp) return;

	if (bmp->palette)
	{
		free(bmp->palette);
		bmp->palette = nullptr;
	}

	if (bmp->data)
	{
		free(bmp->data);
		bmp->data = nullptr;
	}

	free(bmp);
}