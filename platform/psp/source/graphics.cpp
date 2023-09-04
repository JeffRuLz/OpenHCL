#include "graphics.hpp"
#include <malloc.h>
#include "../../../source/bmp.hpp"
#include <string.h>

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include "vram.h"

typedef struct _Vertex
{
	float u, v;
	unsigned int color;
	float x, y, z;
} Vertex;

typedef struct Texture
{
	int format;
	int mipmap;
	int width, height, stride;
	const void* data;
} Texture;

#define BUF_WIDTH 	 512
#define SCR_WIDTH 	 480
#define SCR_HEIGHT 	 272
#define PIXEL_SIZE 	 4 // change this if you change to another screenmode
#define FRAME_SIZE   (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE    (BUF_WIDTH SCR_HEIGHT * 2) // zbuffer seems to be 16-bit?
#define COLOR_FORMAT GU_PSM_5551
#define TEX_FORMAT 	 GU_COLOR_5551

#define RGB8toRGB5A1(R,G,B) ( (1 << 15) | (((B) >> 3) << 10) | (((G) >> 3) << 5) | ((R) >> 3) )

static unsigned int __attribute__((aligned(16))) list[262144];

static Vertex __attribute__((aligned(16))) screen_vertices[2] =
{
	{ 0, 0, 0xff7f0000, 80, 16, 0},
	{ 320, 240, 0xff000000, 320+80, 240+16, 0 }
};

static 		 void* frameBuffer  = (void*)0;
static const void* doubleBuffer = (void*)0x44000;
static const void* renderTarget = (void*)0x88000;
static const void* depthBuffer  = (void*)0x110000;

static Texture offscreenTexture;

//static char screenMode = 0;
static bool fullscreen = false;
static float scroll_y = 0;

static Vertex* vBuf;
static Texture* currentTex = nullptr;
static int texCount = 0;

static void drawVertexBuffer()
{
	if (texCount <= 0)
		return;

	if (currentTex == nullptr)
	{
		sceGuDisable(GU_TEXTURE_2D);
	}
	else
	{
		sceGuTexMode(COLOR_FORMAT, 0, 0, GU_TRUE);
		sceGuTexImage(0, currentTex->width, currentTex->height, currentTex->width, currentTex->data);	
		sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
		sceGuTexScale(1,1);
		sceGuTexOffset(0,0);
		sceGuEnable(GU_TEXTURE_2D);
	}

	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, texCount, NULL, vBuf);

	currentTex = nullptr;
	texCount = 0;
	
	vBuf = (Vertex*)sceGuGetMemory(sizeof(Vertex) * 500);
}

int gfx_Init()
{
	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(COLOR_FORMAT,frameBuffer,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)doubleBuffer,BUF_WIDTH);
	sceGuDepthBuffer((void*)depthBuffer,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);

	//Blending
	sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD,GU_SRC_ALPHA,GU_ONE_MINUS_SRC_ALPHA,0,0);	

	sceGuFinish();
	sceGuSync(0,0);

	return 0;
}

void gfx_Exit()
{
	sceGuTerm();
}

int gfx_Start()
{
	offscreenTexture.format = COLOR_FORMAT;
	offscreenTexture.mipmap = 0;
	offscreenTexture.width  = 512;
	offscreenTexture.height = 256;
	offscreenTexture.stride = 512;
	offscreenTexture.data   = (void*)((int)sceGeEdramGetAddr() + (int)renderTarget);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

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

bool gfx_GetFullscreen()
{
	return fullscreen;
}

void gfx_SetFullscreen(bool full)
{
	fullscreen = full;

	if (fullscreen == false)
	{
		//1:1
		screen_vertices[0].x = 80;
		screen_vertices[0].y = 16;

		screen_vertices[1].x = 320+80;
		screen_vertices[1].y = 240+16;
	}
	else if (fullscreen == true)
	{
		//fit
		screen_vertices[0].x = 59;
		screen_vertices[0].y = 0;

		screen_vertices[1].x = 362+59;
		screen_vertices[1].y = SCR_HEIGHT;
	}
}

void gfx_FrameStart()
{
	sceGuStart(GU_DIRECT,list);

	sceGuDrawBufferList(COLOR_FORMAT,(void*)renderTarget,offscreenTexture.stride);

	// setup viewport
	sceGuOffset(2048 - (offscreenTexture.width/2),2048 - (offscreenTexture.height/2));
	sceGuViewport(2048,2048,offscreenTexture.width,offscreenTexture.height);

	// clear screen
	sceGuClearColor(0xff000000);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

	vBuf = (Vertex*)sceGuGetMemory(sizeof(Vertex) * 1000);
}

void gfx_FrameEnd()
{
	drawVertexBuffer();

	// set frame buffer
	sceGuDrawBufferList(COLOR_FORMAT,(void*)frameBuffer,BUF_WIDTH);

	// setup viewport
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	
	// clear screen
	//sceGuClearColor(0xff554433);
	sceGuClearColor(0xff000000);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

	//
	{
		// setup texture
		sceGuTexMode(offscreenTexture.format,0,0,0);
		sceGuTexImage(offscreenTexture.mipmap,offscreenTexture.width,offscreenTexture.height,offscreenTexture.stride,offscreenTexture.data);
		sceGuTexFunc(GU_TFX_ADD,GU_TCC_RGB);
		sceGuTexFilter(GU_LINEAR,GU_LINEAR);
		sceGuTexScale(1.0f,1.0f);
		sceGuTexOffset(0.0f,0.0f);
		sceGuAmbientColor(0xffffffff);

		sceGuEnable(GU_TEXTURE_2D);

		// draw screen
		sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,2,0,screen_vertices);

		sceGuDisable(GU_TEXTURE_2D);
	}

	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	frameBuffer = sceGuSwapBuffers();
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

static void swizzle(u8* out, const u8* in, unsigned int width, unsigned int height)
{
   unsigned int blockx, blocky;
   unsigned int j;
 
   unsigned int width_blocks = (width / 16);
   unsigned int height_blocks = (height / 8);
 
   unsigned int src_pitch = (width-16)/4;
   unsigned int src_row = width * 8;
 
   const u8* ysrc = in;
   u32* dst = (u32*)out;
 
   for (blocky = 0; blocky < height_blocks; ++blocky)
   {
      const u8* xsrc = ysrc;
      for (blockx = 0; blockx < width_blocks; ++blockx)
      {
         const u32* src = (u32*)xsrc;
         for (j = 0; j < 8; ++j)
         {
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            *(dst++) = *(src++);
            src += src_pitch;
         }
         xsrc += 16;
     }
     ysrc += src_row;
   }
}

static inline int powOfTwo(int in)
{
	int out = 8;
	while (out < in)
		out *= 2;
	return out;
}

Surface* gfx_LoadSurface(FILE* f)
{
	if (!f)
		return nullptr;

	Bitmap* bmp = nullptr;
	Surface* s = nullptr;
	Texture* tex = nullptr;
	unsigned char* temp = nullptr;

	bmp = loadBitmap(f);

	if (bmp == nullptr)
	{
		#ifdef _DEBUG
		printf("Error loading bitmap\n");
		#endif
	}
	else
	{
		tex = (Texture*)malloc(sizeof(Texture));

		if (tex == nullptr)
		{
			#ifdef _DEBUG
			printf("Error creating surface\n");
			#endif
		}
		else
		{
			s = (Surface*)malloc(sizeof(Surface));
			s->width = bmp->width;
			s->height = bmp->height;
			s->data = tex;

			tex->format = TEX_FORMAT;
			tex->mipmap = 0;
			tex->width = powOfTwo(bmp->width);
			tex->height = powOfTwo(bmp->height);
			tex->stride = tex->width;
			tex->data = (void*)malloc(tex->width*tex->height*2);
			temp = (unsigned char*)malloc(tex->width*tex->height*2);

			//load pixels
			if (bmp->bitCount == 8)
			{
				unsigned char pix, r, g, b;
				unsigned char* ptr = temp;
				unsigned int col;

				unsigned int i = 0;
				for (int y = 0; y < tex->height; y++)
				{
					for (int x = 0; x < tex->width; x++)
					{
						if (x >= s->width || y >= s->height)
							ptr += 2;
						else
						{
							pix = bmp->data[i];

							b = bmp->palette[pix * 4 + 0];
							g = bmp->palette[pix * 4 + 1];
							r = bmp->palette[pix * 4 + 2];						

							if (r == 0 && g == 0 && b == 0)
								col = 0x00000000;
							else
								col = RGB8toRGB5A1(r,g,b);

							*(ptr++) = (unsigned char)(col & 0x00FF);
							*(ptr++) = (unsigned char)((col & 0xFF00) >> 8);

							i++;
						}
					}
				}
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

	swizzle((unsigned char*)tex->data, temp, tex->width*2, tex->height);
	free(temp);

	return s;
}

void gfx_FreeSurface(Surface* s)
{
	if (s)
	{
		if (s->data)
		{
			Texture* tex = (Texture*)s->data;

			if (tex->data)
			{
				free((void*)tex->data);
				tex->data = nullptr;
			}

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

	drawVertexBuffer();

	Vertex* v = (Vertex*)sceGuGetMemory(sizeof(Vertex)*2);// &vBuf[texCount];

	v[0].u = 0;
	v[0].v = 0;
	v[0].color = (0xff << 24) | (c.b << 16) | (c.g << 8) | (c.r);
	v[0].x = x1;
	v[0].y = y1;
	v[0].z = 0.f;

	v[1].u = 0;
	v[1].v = 0;
	v[1].color = v[0].color;
	v[1].x = x2;
	v[1].y = y2;
	v[1].z = 0.f;

	sceGuDisable(GU_TEXTURE_2D);
   sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, 2, NULL, v);
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
	if (!s) return;
	if (!s->data) return;

	y -= scroll_y;

	Texture* tex = (Texture*)s->data;

	if (tex != currentTex)
		drawVertexBuffer();
	currentTex = tex;

	Vertex* v = &vBuf[texCount];

	v[0].u = cropx;
	v[0].v = cropy;
	v[0].color = 0xffffffff;
	v[0].x = (int)x;
	v[0].y = (int)y;
	v[0].z = 0.f;

	v[1].u = cropx + cropw;
	v[1].v = cropy + croph;
	v[1].color = v[0].color;
	v[1].x = (int)x + cropw;
	v[1].y = (int)y + croph;
	v[1].z = 0.f;

	texCount += 2;
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

	//allocate vertex buffer
	tm->data = (void*)malloc(sizeof(Vertex) * width * height * 2);
	Vertex* v = (Vertex*)tm->data;

	int i = 0;
	unsigned short tile;
	int cropx;
	int cropy;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			tile = data[x+y*width];

			cropx = (tile * 20) % s->width;
			cropy = ((tile * 20) / s->width) * 20;

			v[i].u = cropx;
			v[i].v = cropy;
			v[i].color = 0xffffffff;
			v[i].x = x*20 - 60;
			v[i].y = y*20 - 60;
			v[i].z = 0.f;

			v[i+1].u = cropx + 20;
			v[i+1].v = cropy + 20;
			v[i+1].color = v[i].color;
			v[i+1].x = v[i].x + 20;
			v[i+1].y = v[i].y + 20;
			v[i+1].z = 0.f;

			if (tile == 0)
			{
				v[i].x = 0;
				v[i].y = 0;
				v[i+1].x = 0;
				v[i+1].y = 0;
			}

			i+=2;
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

	y -= scroll_y;

	Texture* tex = (Texture*)tm->image->data;
	Vertex* vin = (Vertex*)tm->data;

	int size = tm->width * tm->height * 2;
	Vertex* v = (Vertex*)sceGuGetMemory(sizeof(Vertex) * size);

	for (int i = 0; i < size; i++)
	{
		v[i] = vin[i];
		v[i].x += x;
		v[i].y += y;
	}

	sceGuTexMode(COLOR_FORMAT, 0, 0, GU_TRUE);
	sceGuTexImage(0, tex->width, tex->height, tex->width, tex->data);
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	sceGuTexScale(1,1);
	sceGuTexOffset(0,0);

	sceGuEnable(GU_TEXTURE_2D);
	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D, size, NULL, v);
}