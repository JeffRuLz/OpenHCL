#include "graphics.hpp"
#include <citro2d.h>
#include <malloc.h>
#include <string.h>
#include "../../../source/bmp.hpp"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

static C3D_RenderTarget* fb[3] = { nullptr };
static C3D_Tex* blankTex = nullptr;

static float scroll_y = 0;

int gfx_Init()
{
	gfxInitDefault();
	gfxSet3D(true);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screens
	fb[0] = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	fb[1] = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);

	#ifndef _DEBUG
	  fb[2] = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	#else
	  consoleInit(GFX_BOTTOM, NULL);
	#endif

	blankTex = new C3D_Tex;
	C3D_TexInit(blankTex, 8, 8, GPU_RGBA8);
	u32* blankPix = (u32*)linearAlloc(8*8*sizeof(u32));
	for (int i = 0; i < 8*8; i++)
		blankPix[i] = 0xFFFFFFFF;
	C3D_TexUpload(blankTex, blankPix);
	linearFree(blankPix);

	return 0;
}

void gfx_Exit()
{
	C3D_TexDelete(blankTex);
	blankTex = nullptr;

	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

int gfx_Start()
{
	return 0;
}

void gfx_End()
{

}

float gfx_GetScrollY()
{
	return scroll_y;
}

void gfx_SetScrollY(float n)
{
	scroll_y = n;
}

void gfx_FrameStart()
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	gfx_FrameSelect(0);
}

void gfx_FrameEnd()
{
	C3D_FrameEnd(0);
}

void gfx_FrameSelect(int n)
{
	C2D_TargetClear(fb[n], C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
	C2D_SceneBegin(fb[n]);

	if (n < 2)
	{
		C3D_SetViewport(0, -40, 240.0, 400.0);
		C3D_SetScissor(GPU_SCISSOR_NORMAL, 0, 40, 0, 360);
	}
}

Surface* gfx_LoadSurface(const char* fname)
{
	Surface* s = nullptr;
	FILE* f = fopen(fname, "rb");

	if (!f)
	{
		printf("Could not open %s\n", fname);
	}
	else
	{
		s = gfx_LoadSurface(f);
		fclose(f);
	}

	return s;
}

static inline int powOfTwo(int in)
{
	int out = 8;
	while (out < in)
		out *= 2;
	return out;
}

static inline void swapPixel(u32 &a, u32 &b)
{
	u32 tmp = a;
	a = b;
	b = tmp;
}

static inline void swizzle(u32* p)
{
	// swizzle foursome table
	static const unsigned char table[][4] =
	{
		{  2,  8, 16,  4, },
		{  3,  9, 17,  5, },
		{  6, 10, 24, 20, },
		{  7, 11, 25, 21, },
		{ 14, 26, 28, 22, },
		{ 15, 27, 29, 23, },
		{ 34, 40, 48, 36, },
		{ 35, 41, 49, 37, },
		{ 38, 42, 56, 52, },
		{ 39, 43, 57, 53, },
		{ 46, 58, 60, 54, },
		{ 47, 59, 61, 55, }
	};

	for (const auto &entry : table)
	{
		u32 tmp = p[entry[0]];
		p[entry[0]] = p[entry[1]];
		p[entry[1]] = p[entry[2]];
		p[entry[2]] = p[entry[3]];
		p[entry[3]] = tmp;
	}

	swapPixel(p[12], p[18]);
	swapPixel(p[13], p[19]);
	swapPixel(p[44], p[50]);
	swapPixel(p[45], p[51]);
}

Surface* gfx_LoadSurface(FILE* f)
{
	if (!f)
		return nullptr;

	Bitmap* bmp = nullptr;
	Surface* s = nullptr;
	C3D_Tex* tex = nullptr;

	bmp = loadBitmap(f);

	if (bmp == nullptr)
	{
		#ifdef _DEBUG
		printf("Error loading bitmap\n");
		#endif
	}
	else
	{
		int w = powOfTwo(bmp->width);
		int h = powOfTwo(bmp->height);

		tex = new C3D_Tex;
		C3D_TexInit(tex, w, h, GPU_RGBA8);

		s = (Surface*)malloc(sizeof(Surface));
		s->width = bmp->width;
		s->height = bmp->height;
		s->data = (void*)tex;

		//load pixels
		u32* out = (u32*)linearAlloc(w * h * sizeof(u32));
		u32* pOut = out;
		unsigned char pix;

		int x = 0;
		int y = 0;

		for (int i = 0; i < w * h;)
		{
			//load 8x8 chunks
			unsigned char r, g, b, a;

			u32 tile[8*8] = { 0 };
			u32* ptr = tile;

			for (int ty = 0; ty < 8; ty++)
			{
				for (int tx = 0; tx < 8; tx++)
				{
					r = g = b = a = 0xFF;

					if (x < s->width && y < s->height)
					{
						pix = bmp->data[x + y * s->width];

						b = bmp->palette[pix*4+0];
						g = bmp->palette[pix*4+1];
						r = bmp->palette[pix*4+2];
						a = 0xFF;

						if (r == 0 && g == 0 && b == 0)
							a = 0;
					}

					*(ptr++) = (r << 24) | (g << 16) | (b << 8) | a;

					x += 1;
					i += 1;
				}

				//return to left, and down one row
				x -= 8;
				y += 1;
			}

			//return to top, move to next tile
			y -= 8;
			x += 8;

			//if finished with the rightmost tile, return to the left and down one tile
			if (x >= w - 1)
			{
				x = 0;
				y += 8;
			}

			swizzle(tile);

			for (int a = 0; a < 8*8; a++)
				*(pOut++) = tile[a];
		}

		C3D_TexUpload(tex, out);
		linearFree(out);
	}

	freeBitmap(bmp);

	return s;
}

void gfx_FreeSurface(Surface* s)
{
	if (s)
	{
		if (s->data)
		{
			C3D_Tex* tex = (C3D_Tex*)s->data;
			C3D_TexDelete(tex);
			s->data = nullptr;
		}

		free(s);
	}
}

void gfx_DrawRect(float x1, float y1, float x2, float y2, Color c)
{
	y1 -= scroll_y;
	y2 -= scroll_y;

	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 < 0) x2 = 0;
	if (y2 < 0) y2 = 0;

	u32 col = C2D_Color32(c.r, c.g, c.b, 0xFF);

	Tex3DS_SubTexture subtex;
	subtex.width  = 8;
	subtex.height = 8;
	subtex.left = 0;
	subtex.top  = 1;
	subtex.right  = 0;
	subtex.bottom = 0;

	C2D_Image img;
	img.tex = blankTex;
	img.subtex = &subtex;

	C2D_DrawParams params;
	params.pos.x = (int)x1;
	params.pos.y = (int)y1;
	params.pos.w = x2 - x1;
	params.pos.h = y2 - y1;
	params.center.x = 0;
	params.center.y = 0;
	params.depth = 0;
	params.angle = 0;

	C2D_ImageTint tint =
	{
		(C2D_Tint){ col, 1.f },
		(C2D_Tint){ col, 1.f },
		(C2D_Tint){ col, 1.f },
		(C2D_Tint){ col, 1.f }
	};

	C2D_DrawImage(img, &params, &tint);
}

void gfx_DrawSurface(Surface* s, float x, float y)
{
	if (!s) return;
	gfx_DrawSurfacePart(s, x, y, 0, 0, s->width, s->height);
}

void gfx_DrawTile(Surface* s, float x, float y, int tilew, int tileh, int index)
{
	if (!s || index < 0)
		return;

	if (s->width <= 0)
		return;

	int cropx = (index * tilew) % s->width;
	int cropy = ((index * tilew) / s->width) * tileh;

	gfx_DrawSurfacePart(s, x, y, cropx, cropy, tilew, tileh);
}

void gfx_DrawSurfacePart(Surface* s, float x, float y, int cropx, int cropy, int cropw, int croph)
{
	if (!s) return;
	if (!s->data) return;

	y -= scroll_y;

	C3D_Tex* tex = (C3D_Tex*)s->data;
	
	Tex3DS_SubTexture subtex;
	subtex.width  = s->width;
	subtex.height = s->height;
	subtex.left = (float)cropx / (float)tex->width;
	subtex.top  = 1 - (float)cropy / (float)tex->height;
	subtex.right  = (float)(cropx + cropw) / (float)tex->width;
	subtex.bottom = 1 - (float)(cropy + croph) / (float)tex->height;

	C2D_Image img;
	img.tex = tex;
	img.subtex = &subtex;

	C2D_DrawParams params;
	params.pos.x = (int)x;
	params.pos.y = (int)y;
	params.pos.w = cropw;
	params.pos.h = croph;
	params.center.x = 0;
	params.center.y = 0;
	params.depth = 0;
	params.angle = 0;

	C2D_DrawImage(img, &params);
}

//
Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data)
{
	if (!s) return nullptr;
	if (!s->data) return nullptr;

	Tilemap* tm = (Tilemap*)malloc(sizeof(Tilemap));

	tm->width = width;
	tm->height = height;
	tm->image = s;

	tm->data = (unsigned short*)malloc(width * height * sizeof(unsigned short));
	unsigned short* in = (unsigned short*)tm->data;

	int i = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			in[i] = data[x+y*width];
			i += 1;
		}
	}

	return tm;
}

void gfx_FreeTilemap(Tilemap* tm)
{
	if (tm)
	{
		tm->width = 0;
		tm->height = 0;
		tm->image = nullptr;

		if (tm->data)
		{
			free(tm->data);
			tm->data = nullptr;
		}

		free(tm);
	}
}

void gfx_DrawTilemap(Tilemap* tm, float x, float y)
{
	if (!tm) return;

	x -= 60;
	y -= 60;

	y -= scroll_y;

	int tile = 0;
	for (int ty = 0+3; ty < tm->height-3; ty++)
	{
		for (int tx = 0+3; tx < tm->width-3; tx++)
		{
			tile = ((unsigned short*)tm->data)[tx + ty * 22];
			if (tile != 0)
				gfx_DrawTile(tm->image, tx*20+(int)x, ty*20+(int)y, 20, 20, tile);
		}
	}
}