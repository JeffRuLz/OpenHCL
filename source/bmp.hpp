#pragma once

#include <stdio.h>

typedef struct
{
	int width;
	int height;
	int bitCount;
	unsigned char* palette;
	unsigned char* data;
} Bitmap;

Bitmap* loadBitmap(FILE* f);
void freeBitmap(Bitmap* bmp);