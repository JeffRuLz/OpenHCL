#include "graphics.hpp"
#include <SDL2/SDL.h>
#include <malloc.h>
#include "../../../source/bmp.hpp"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static int scale = 3;
static bool vsync = true;

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* screenBuffer = nullptr;

static float scroll_y = 0;

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
	int w = SCREEN_WIDTH * scale;
	int h = SCREEN_HEIGHT * scale;

	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return -1;
	}

	Uint32 flags = SDL_RENDERER_ACCELERATED;
	if (vsync) flags |= SDL_RENDERER_PRESENTVSYNC;

	renderer = SDL_CreateRenderer(window, -1, flags);
	if (renderer == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return -2;
	}

	screenBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA5551, SDL_TEXTUREACCESS_TARGET, w, h);
	if (screenBuffer == nullptr)
	{
		printf("Screen Buffer could not be created! SDL Error: %s\n", SDL_GetError());
		return -3;
	}

	return 0;
}

void gfx_End()
{
	SDL_DestroyTexture(screenBuffer);
	screenBuffer = nullptr;

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;
}

void gfx_FrameStart()
{
	SDL_SetRenderTarget(renderer, screenBuffer);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
}

void gfx_FrameEnd()
{
	SDL_SetRenderTarget(renderer, nullptr);
	SDL_RenderCopy(renderer, screenBuffer, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

float gfx_GetScrollY()
{
	return scroll_y;
}

void gfx_SetScrollY(float n)
{
	scroll_y = n;
}

bool gfx_GetFullscreen()
{
	return (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void gfx_SetFullscreen(bool full)
{
	SDL_SetWindowFullscreen(window, ((full)? SDL_WINDOW_FULLSCREEN_DESKTOP: 0));
}

bool gfx_GetVsync()
{
	return vsync;
}

void gfx_SetVsync(bool on)
{
	vsync = on;
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
		temp = SDL_CreateRGBSurfaceWithFormat(0, bmp->width, bmp->height, 32, SDL_PIXELFORMAT_RGBA32);

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
						pix[3] = 0xFF;

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

			SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0, 0, 0));

			s = (Surface*)malloc(sizeof(Surface));

			s->width = temp->w;
			s->height = temp->h;

			s->data = (void*)SDL_CreateTextureFromSurface(renderer, temp);
			SDL_FreeSurface(temp);
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
			SDL_DestroyTexture((SDL_Texture*)s->data);
			s->data = nullptr;
		}

		free(s);
	}
}

void gfx_DrawRect(float x1, float y1, float x2, float y2, Color c)
{
	y1 -= scroll_y;
	y2 -= scroll_y;

	int dx1 = x1 * (float)scale;
	int dy1 = y1 * (float)scale;
	int dx2 = x2 * (float)scale;
	int dy2 = y2 * (float)scale;

	SDL_Rect r;
	
	r.x = dx1;
	r.y = dy1;
	r.w = dx2 - dx1;
	r.h = dy2 - dy1;

	SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 0xFF);
	SDL_RenderFillRect(renderer, &r);
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

	dest.x = x * (float)scale;
	dest.y = y * (float)scale;
	dest.w = cropw * scale;
	dest.h = croph * scale;

	SDL_RenderCopy(renderer, (SDL_Texture*)s->data, &crop, &dest);
}

//
Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data)
{
	Tilemap* tm = (Tilemap*)malloc(sizeof(Tilemap));

	tm->width = width;
	tm->height = height;
	tm->image = s;

	tm->data = (void*)SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA5551, SDL_TEXTUREACCESS_TARGET, width*20, height*20);
	SDL_Texture* tex = (SDL_Texture*)tm->data;

	SDL_SetRenderTarget(renderer, tex);
	{
		//Clear texture
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		
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

					SDL_RenderCopy(renderer, (SDL_Texture*)s->data, &crop, &dest);
				}
			}
		}
	}
	SDL_SetRenderTarget(renderer, nullptr);

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
			SDL_Texture* tex = (SDL_Texture*)tm->data;
			SDL_DestroyTexture(tex);
			tm->data = nullptr;
		}

		free(tm);
	}
}

void gfx_DrawTilemap(Tilemap* tm, float x, float y)
{
	y -= scroll_y;

	SDL_Rect dest;

	dest.x = x * (float)scale;
	dest.y = y * (float)scale;
	dest.w = tm->width * 20 * scale;
	dest.h = tm->height * 20 * scale;

	SDL_RenderCopy(renderer, (SDL_Texture*)tm->data, nullptr, &dest);
}