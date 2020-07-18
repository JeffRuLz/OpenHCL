#pragma once

#include <stdio.h>

typedef struct
{
	unsigned char r, g, b;
} Color;

typedef struct
{
	int width;
	int height;
	void* data;
} Surface;

typedef struct
{
	int width;
	int height;
	Surface* image;
	void* data;
} Tilemap;

int gfx_Init();
void gfx_Exit();

int gfx_Start();
void gfx_End();

void gfx_FrameStart();
void gfx_FrameEnd();

float gfx_GetScrollY();
void gfx_SetScrollY(float n);

#ifdef _HAS_FULLSCREEN
bool gfx_GetFullscreen();
void gfx_SetFullscreen(bool full);
#endif

#ifdef _HAS_VSYNC
bool gfx_GetVsync();
void gfx_SetVsync(bool on);
#endif

#ifdef _HAS_VIDEOMODE
int gfx_GetVideoMode(char* str = nullptr);
void gfx_SetVideoMode(int n);
#endif

#ifdef _3DS
void gfx_FrameSelect(int n);
#endif

Surface* gfx_LoadSurface(const char* fname);
Surface* gfx_LoadSurface(FILE* f);
void gfx_FreeSurface(Surface* s);

void gfx_DrawRect(float x1, float y1, float x2, float y2, Color c);

void gfx_DrawSurface(Surface* s, float x, float y);
void gfx_DrawTile(Surface* s, float x, float y, int tilew, int tileh, int index);
void gfx_DrawSurfacePart(Surface* s, float x, float y, int cropx, int cropy, int cropw, int croph);

Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data);
void gfx_FreeTilemap(Tilemap* tm);
void gfx_DrawTilemap(Tilemap* tm, float x, float y);