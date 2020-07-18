#include "graphics.hpp"
#include <SDL/SDL.h>
#include <malloc.h>
#include "../../bmp.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_BPP 16

static float scroll_y = 0;
static SDL_Surface* screen = nullptr;

static long time = 0;
static long lastTime = 0;

int gfx_Init()
{
	int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);

	if (ret < 0)
	{
		printf("SDL Video could not initialize! SDL_Error: %s\n", SDL_GetError());
		return ret;
	}

	return 0;
}

void gfx_Exit()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int gfx_Start()
{
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE|SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Open HCL", NULL);
	SDL_ShowCursor(SDL_DISABLE);

	time = SDL_GetTicks();
	lastTime = time;

	return 0;
}

void gfx_End()
{
	SDL_FreeSurface(screen);
	screen = nullptr;
}

void gfx_FrameStart()
{
	//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	//memset(screen->pixels, 0, SCREEN_WIDTH*SCREEN_HEIGHT*(SCREEN_BPP>>3));
}

void gfx_FrameEnd()
{
	time = SDL_GetTicks();

	SDL_Flip(screen);

	if (time - lastTime < 1000 / 60)
		SDL_Delay((1000/60) - (time - lastTime));
	//SDL_Delay(1);

	lastTime = SDL_GetTicks();
}

float gfx_GetScrollY()
{
	return scroll_y;
}

void gfx_SetScrollY(float n)
{
	scroll_y = n;
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

#define RGB24toRGB16(R,G,B) ( ((R)>>3)<<11 | ((G)>>2)<<5 | ((B)>>3) ) 

Surface* gfx_LoadSurface(FILE* f)
{
	if (!f)
		return nullptr;

	Bitmap* bmp = nullptr;
	Surface* s = nullptr;

	bmp = loadBitmap(f);

	if (bmp == nullptr)
	{
		#ifdef _DEBUG
		printf("Error loading bitmap\n");
		#endif
	}
	else
	{
		s = (Surface*)malloc(sizeof(Surface));
		s->width = bmp->width;
		s->height = bmp->height;
		int size = s->width * s->height * (SCREEN_BPP >> 3);
		s->data = (void*)malloc(size);
		memset(s->data, 0, size);

		if (s == nullptr)
		{
			#ifdef _DEBUG
			printf("Error creating surface: %s\n", SDL_GetError());
			#endif
		}
		else
		{
			if (bmp->bitCount == 8)
			{
				unsigned short col;
				unsigned char r, g, b;
				unsigned char* ptr = bmp->data;
				unsigned char* pix = (unsigned char*)s->data;

				for (int y = 0; y < bmp->height; y++)
				{
					for (int x = 0; x < bmp->width; x++)
					{
						b = bmp->palette[(*ptr * 4)];
						g = bmp->palette[(*ptr * 4) + 1];
						r = bmp->palette[(*ptr * 4) + 2];

						col = RGB24toRGB16(r,g,b);

						pix[0] = (col & 0x00FF);
						pix[1] = (col & 0xFF00) >> 8;

						pix += 2;
						ptr += 1;
					}
				}
			}
			else if (bmp->bitCount == 24)
			{
				#ifdef _DEBUG
				printf("gfx_LoadSurface() 24 bitcount unsupported\n");
				#endif
			}
			else
			{
				#ifdef _DEBUG
				printf("gfx_LoadSurface() unsupported bitcount\n");
				#endif
			}
		}		
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
			free(s->data);
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
	if (x2 > SCREEN_WIDTH-1) x2 = SCREEN_WIDTH-1;
	if (y2 > SCREEN_HEIGHT-1) y2 = SCREEN_HEIGHT-1;

	unsigned short col = RGB24toRGB16(c.r, c.g, c.b);
	unsigned char* pix;

	for (int y = y1; y < y2; y++)
	{
		pix = (unsigned char*)screen->pixels + (((int)x1 + (y * SCREEN_WIDTH)) * 2);

		for (int x = x1; x < x2; x++)
		{
			pix[0] = (col & 0x00FF);
			pix[1] = (col & 0xFF00) >> 8;

			pix += 2;
		}
	}
}

void gfx_DrawSurface(Surface* s, float x, float y)
{
	if (!s)
		return;

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
	if (!s)	return;
	if (!s->data) return;

	y -= scroll_y;

	int srcx1 = cropx;
	int srcy1 = cropy;
	int srcx2 = cropx + cropw;
	int srcy2 = cropy + croph;

	int dstx1 = x;
	int dsty1 = y;
	int dstx2 = x + cropw;
	int dsty2 = y + croph;

	if (dstx1 < 0) { srcx1 -= dstx1; dstx1 = 0; }
	if (dsty1 < 0) { srcy1 -= dsty1; dsty1 = 0; }
	if (dstx2 > SCREEN_WIDTH-1) { dstx2 = SCREEN_WIDTH-1; }
	if (dsty2 > SCREEN_HEIGHT-1) { dsty2 = SCREEN_HEIGHT-1; }

	unsigned char* col = (unsigned char*)s->data;
	unsigned char* pix;

	for (int iy = dsty1; iy < dsty2; iy++)
	{
		col = (unsigned char*)s->data + ((srcx1 + (srcy1 * s->width)) * 2);
		pix = (unsigned char*)screen->pixels + ((dstx1 + (iy * SCREEN_WIDTH)) * 2);

		for (int ix = dstx1; ix < dstx2; ix++)
		{
			if (col[0] != 0 || col[1] != 0)
			{	
				pix[0] = col[0];
				pix[1] = col[1];
			}

			pix += 2;
			col += 2;
		}

		srcy1 += 1;
	}

	//gfx_DrawRect(x, y, x+cropw, y+croph, (Color){0,0,0xFF});

/*
	SDL_Rect crop, dest;

	crop.x = cropx;
	crop.y = cropy;
	crop.w = cropw;
	crop.h = croph;

	dest.x = x;
	dest.y = y;
	dest.w = cropw;
	dest.h = croph;

	SDL_BlitSurface((SDL_Surface*)s->data, &crop, screen, &dest);
*/
}

Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data)
{
	if (!s) return nullptr;
	if (!s->data) return nullptr;

	Tilemap* tm = (Tilemap*)malloc(sizeof(Tilemap));

	tm->width = width;
	tm->height = height;
	tm->image = s;
	tm->data = (void*)malloc(width*height);

	memcpy(tm->data, data, width*height);

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
	if (!tm->image) return;
	if (!tm->data) return;

	gfx_DrawSurface(tm->image, x, y);
/*
	for (int ty = 0+3; ty < tm->height-3; ty++)
	{
		unsigned char* ptr = ((unsigned char*)tm->data) + (3 + ty * tm->width);
		for (int tx = 0+3; tx < tm->width-3; tx++)
		{
			gfx_DrawTile(tm->image, tx*20-60, ty*20-60, 20, 20, *ptr);
			ptr += 1;
		}
	}
*/
	//gfx_DrawSurface(backSurf, x, y);

	/*
	if (!backSurf) return;
	if (!tm) return;
	if (!tm->data) return;

	y -= scroll_y;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = tm->width * 20;
	dest.h = tm->height * 20;

	SDL_BlitSurface(backSurf, NULL, screen, &dest);
	*/
}

/*
#include "graphics.hpp"
#include <SDL/SDL.h>
#include <malloc.h>
#include "../../bmp.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_BPP 16

static float scroll_y = 0;

static SDL_Surface* screen = nullptr;

static long time = 0;
static long lastTime = 0;

int gfx_Init()
{
	int ret = SDL_InitSubSystem(SDL_INIT_VIDEO);

	if (ret < 0)
	{
		printf("SDL Video could not initialize! SDL_Error: %s\n", SDL_GetError());
		return ret;
	}

	return 0;
}

void gfx_Exit()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int gfx_Start()
{
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE|SDL_DOUBLEBUF);
	SDL_WM_SetCaption("Open HCL", NULL);
	SDL_ShowCursor(SDL_DISABLE);

	time = SDL_GetTicks();
	lastTime = time;

	return 0;
}

void gfx_End()
{
	SDL_FreeSurface(screen);
	screen = nullptr;
}

void gfx_FrameStart()
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
}

void gfx_FrameEnd()
{
	time = SDL_GetTicks();

	SDL_Flip(screen);

//#ifdef _NO_LERP
//	if (time - lastTime < 1000 / 60)
//		SDL_Delay((1000/60) - (time - lastTime));
//#else
	SDL_Delay(1);
//#endif

	lastTime = SDL_GetTicks();
}

float gfx_GetScrollY()
{
	return scroll_y;
}

void gfx_SetScrollY(float n)
{
	scroll_y = n;
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

Surface* gfx_LoadSurface(FILE* f)
{
	if (!f)
		return nullptr;

	Bitmap* bmp = nullptr;
	Surface* s = nullptr;
	SDL_Surface* temp = nullptr;

	bmp = loadBitmap(f);

	if (bmp == nullptr)
	{
		#ifdef _DEBUG
		printf("Error loading bitmap\n");
		#endif
	}
	else
	{
		temp = SDL_CreateRGBSurface(SDL_SWSURFACE, bmp->width, bmp->height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_FillRect(temp, NULL, SDL_MapRGB(temp->format, 0, 0, 0));

		if (temp == nullptr)
		{
			#ifdef _DEBUG
			printf("Error creating surface: %s\n", SDL_GetError());
			#endif
		}
		else
		{
			//load pixels
			unsigned char* pix = (unsigned char*)temp->pixels;

			if (bmp->bitCount == 8)
			{
				unsigned char r, g, b;
				unsigned char* ptr = bmp->data;

				for (int y = 0; y < bmp->height; y++)
				{
					for (int x = 0; x < bmp->width; x++)
					{
						b = bmp->palette[(*ptr * 4)];
						g = bmp->palette[(*ptr * 4) + 1];
						r = bmp->palette[(*ptr * 4) + 2];

						pix[0] = r;
						pix[1] = g;
						pix[2] = b;
						pix[3] = ((r==0 && g==0 && b==0)? 0: 0xFF);

						pix += 4;
						ptr += 1;
					}
				}
			}
			else if (bmp->bitCount == 24)
			{
				#ifdef _DEBUG
				printf("gfx_LoadSurface() 24 bitcount unsupported\n");
				#endif
			}
			else
			{
				#ifdef _DEBUG
				printf("gfx_LoadSurface() unsupported bitcount\n");
				#endif
			}

			SDL_SetColorKey(temp, SDL_SRCCOLORKEY, SDL_MapRGB(temp->format, 0, 0, 0));

			s = (Surface*)malloc(sizeof(Surface));

			s->width = temp->w;
			s->height = temp->h;

			s->data = (void*)SDL_DisplayFormat(temp);
			SDL_FreeSurface(temp);

			SDL_Surface* surf = (SDL_Surface*)s->data;
		}		
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
			SDL_FreeSurface((SDL_Surface*)s->data);
			s->data = nullptr;
		}

		free(s);
	}
}

void gfx_DrawRect(float x1, float y1, float x2, float y2, Color c)
{
	y1 -= scroll_y;
	y2 -= scroll_y;

	SDL_Rect r;
	r.x = x1;
	r.y = y1;
	r.w = x2 - x1;
	r.h = y2 - y1;

	SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, c.r, c.g, c.b));
}

void gfx_DrawSurface(Surface* s, float x, float y)
{
	if (!s)
		return;

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
	if (!s)
		return;

	y -= scroll_y;

	SDL_Rect crop, dest;

	crop.x = cropx;
	crop.y = cropy;
	crop.w = cropw;
	crop.h = croph;

	dest.x = x;
	dest.y = y;
	dest.w = cropw;
	dest.h = croph;

	SDL_BlitSurface((SDL_Surface*)s->data, &crop, screen, &dest);
}

//
static SDL_Surface* backSurf = nullptr;

Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data)
{
	Tilemap* tm = (Tilemap*)malloc(sizeof(Tilemap));

	tm->width = width;
	tm->height = height;
	tm->image = s;

	if (backSurf)
	{
		tm->data = (void*)false;
	}
	else
	{
		tm->data = (void*)true;

		SDL_Surface* temp = SDL_CreateRGBSurface(0, width*20, height*20, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
		backSurf = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);		

		SDL_FillRect(backSurf, NULL, SDL_MapRGB(backSurf->format, 0, 0, 0));
		SDL_SetColorKey(backSurf, SDL_SRCCOLORKEY, SDL_MapRGB(backSurf->format, 0, 0, 0));
	}
	
	//
	unsigned short tile = 0;
	SDL_Rect crop, dest;

	crop.w = 20;
	crop.h = 20;

	dest.w = 20;
	dest.h = 20;

	for (int y = 0+3; y < height-3; y++)
	{
		for (int x = 0+3; x < width-3; x++)
		{
			tile = data[x+y*width];

			if (tile != 0)
			{
				crop.x = (tile * 20) % s->width;
				crop.y = ((tile * 20) / s->width) * 20;				

				dest.x = x * 20 - 60;
				dest.y = y * 20 - 60;

				SDL_BlitSurface((SDL_Surface*)s->data, &crop, backSurf, &dest);
			}
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
		tm->data = nullptr;

		free(tm);
	}

	if (backSurf)
	{
		SDL_FreeSurface(backSurf);
		backSurf = nullptr;
	}
}

void gfx_DrawTilemap(Tilemap* tm, float x, float y)
{
	if (!backSurf) return;
	if (!tm) return;
	if (!tm->data) return;

	y -= scroll_y;

	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = tm->width * 20;
	dest.h = tm->height * 20;

	SDL_BlitSurface(backSurf, NULL, screen, &dest);
}
*/