#include "graphics.hpp"
#include <gccore.h>
#include <ogc/tpl.h>
#include <malloc.h>
#include <string.h>
#include "../../../source/bmp.hpp"

typedef struct
{
	GXTexObj* texobj;
	unsigned char* img_ptr;
} GXTexture;

#define DEFAULT_FIFO_SIZE	(256*1024)

static u8 scale = 2;

static u32 fb = 0;
static u32 first_frame = true;
static void* frameBuffer[2] = { NULL, NULL};
static GXRModeObj* rmode = NULL;

static Mtx44 perspective;
static Mtx GXmodelView2D;
static void* gp_fifo = NULL;

static bool widescreen = false;
static bool progressive = false;
static bool pal = false;
static int videomode = 1;

static float scroll_y = 0;

static GXTexture* currentTexture = nullptr;

static void _setupVideo()
{
	f32 yscale;
	u32 xfbHeight;
	GXColor background = {0, 0, 0, 0xff};

	widescreen = (CONF_GetAspectRatio() == CONF_ASPECT_16_9);
	progressive = CONF_GetProgressiveScan();
	pal = (CONF_GetVideo() != CONF_VIDEO_NTSC);

	if (widescreen)
		rmode->viWidth = 720;
/*
	// allocate 2 framebuffers for double buffering
	if (frameBuffer[0] != nullptr)
		free(frameBuffer[0]);
	
	if (frameBuffer[1] != nullptr)
		free(frameBuffer[1]);

	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	*/
	// Initialise the console, required for printf
	//console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	// Tell the video hardware where our display memory is
	VIDEO_ClearFrameBuffer(rmode, frameBuffer[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(rmode, frameBuffer[1], COLOR_BLACK);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	// Make the display visible
	VIDEO_SetBlack(FALSE);
	// Flush the video register changes to the hardware
	VIDEO_Flush();
	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb ^= 1;
/*
	// setup the fifo and then init the flipper
	if (gp_fifo == NULL)
	{
		gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
		memset(gp_fifo,0,DEFAULT_FIFO_SIZE);

		GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
	}
*/
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);
	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	//GX_SetScissor(80*widescreen,0,rmode->fbWidth-80*widescreen,rmode->efbHeight);
	GX_SetScissor(80*widescreen,0,rmode->fbWidth-160*widescreen,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));
	
	if (rmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	GX_InvalidateTexAll();

	if (widescreen)
		guOrtho(perspective,0,479,0,854-1,0,300);
	else
		guOrtho(perspective,0,479,0,639,0,300);
	
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
}

int gfx_Init()
{
	// Initialise the video system
	VIDEO_Init();
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	// setup the fifo and then init the flipper
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);

	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);

	_setupVideo();

	if (progressive)
		videomode = 2;

	return 0;
}

void gfx_Exit()
{

}

int gfx_Start()
{	
	return 0;
}

void gfx_End()
{

}

int gfx_GetVideoMode(char* str)
{
	if (str != nullptr)
	{
		if (videomode == 0)
			sprintf(str, "%dp", rmode->xfbHeight);
		else if (videomode == 1)
			sprintf(str, "%di", rmode->xfbHeight);
		else if (videomode == 2)
			sprintf(str, "%dp", rmode->xfbHeight);
	}

	return videomode;
}

void gfx_SetVideoMode(int n)
{
	if (n == videomode)
		return;

	int dir = (n > videomode) - (n < videomode);

	u32 tvmode = CONF_GetVideo();

	while (1)
	{
		//240p
		if (n == 0 && progressive == false)
		{
			switch (tvmode)
			{
				case CONF_VIDEO_NTSC:
					rmode = &TVNtsc240Ds;
					break;

				case CONF_VIDEO_PAL:
				{
					if (CONF_GetEuRGB60() > 0)
						rmode = &TVEurgb60Hz240Ds;
					else
						rmode = &TVPal264Ds;
				}
				break;

				case CONF_VIDEO_MPAL:
					rmode = &TVMpal240Ds;
			}

			break;
		}
		//480i
		else if (n == 1 && progressive == false)
		{
			switch (tvmode)
			{
				case CONF_VIDEO_NTSC:
					rmode = &TVNtsc480IntDf;
					break;

				case CONF_VIDEO_PAL:
				{
					if (CONF_GetEuRGB60() > 0)
						rmode = &TVEurgb60Hz480IntDf;
					else
						rmode = &TVPal576IntDfScale;
				}
				break;

				case CONF_VIDEO_MPAL:
					rmode = &TVMpal480IntDf;
			}

			break;
		}
		//480p
		else if (n == 2 && progressive == true)
		{
			switch (tvmode)
			{
				case CONF_VIDEO_NTSC:
					rmode = &TVNtsc480Prog;
					break;

				case CONF_VIDEO_PAL:
				{
					if (CONF_GetEuRGB60() > 0)
						rmode = &TVEurgb60Hz480Prog;
					else
						rmode = &TVPal576ProgScale;
				}
				break;

				case CONF_VIDEO_MPAL:
					rmode = &TVMpal480Prog;
			}

			break;
		}

		n += dir;
		if (n < 0) n = 2;
		if (n > 2) n = 0;
	}

	//if (widescreen)
	//	rmode->viWidth = 720;

	if (n != videomode)
	{
		videomode = n;
		_setupVideo();
	}
}

void gfx_FrameStart()
{
	//GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	GX_SetViewport(80*widescreen,0,rmode->fbWidth,rmode->efbHeight,0,1);

	GX_InvVtxCache();
	GX_InvalidateTexAll();

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	guMtxIdentity(GXmodelView2D);
	guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
	GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);

	currentTexture = nullptr;
}

void gfx_FrameEnd()
{
	GX_DrawDone();

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);

	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	if (first_frame) {
		VIDEO_SetBlack(FALSE);
		first_frame = 0;
	}
	VIDEO_Flush();
	VIDEO_WaitVSync();
	fb ^= 1;		// flip framebuffer
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

#define RGB8toRGB5A3(r, g, b) ((0x1 << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3))

Surface* gfx_LoadSurface(FILE* f)
{
	if (!f)
		return nullptr;

	Bitmap* bmp = nullptr;
	Surface* s = nullptr;

	bmp = loadBitmap(f);

	if (bmp == nullptr)
	{
		printf("Error loading bitmap\n");
	}
	else
	{
		s = (Surface*)memalign(32, sizeof(Surface));
		s->width = bmp->width;
		s->height = bmp->height;
		s->data = (GXTexture*)memalign(32, sizeof(GXTexture));

		GXTexture* tex = (GXTexture*)s->data;
		tex->texobj = (GXTexObj*)memalign(32, sizeof(GXTexObj));
		tex->img_ptr = (unsigned char*)memalign(32, s->width * s->height * 2);
		memset(tex->img_ptr, 0, s->width * s->height * 2);

		int tpx = 0;
		int tpy = 0;

		unsigned int col = 0;
		long pix = 0;
		unsigned char r, g, b;

		for (long i = 0; i < s->width * s->height * 2;)
		{
			//load texel
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					r = g = b = 0;
					col = 0;

					if (tpx <= s->width && tpy <= s->height)
					{
						if (bmp->bitCount == 8)
						{
							pix = bmp->data[(tpx + tpy * s->width)]*4;

							b = bmp->palette[pix];
							g = bmp->palette[pix+1];
							r = bmp->palette[pix+2];
						}
						else if (bmp->bitCount == 24)
						{
							pix = (tpx + (tpy * s->width)) * 3;

							b = bmp->data[pix];
							g = bmp->data[pix+1];
							r = bmp->data[pix+2];
						}
					
						if (r != 0 || g != 0 || b != 0)
							col = RGB8toRGB5A3(r,g,b);

						tex->img_ptr[i]   = col >> 8;
						tex->img_ptr[i+1] = col;
					}

					tpx += 1;
					i += 2;
				}

				//Return to left, and down one row
				tpx -= 4;
				tpy++;
			}

			//Return to top, move to next texel right
			tpy -= 4;
			tpx += 4;

			//if finished with the rightmost tile, return to the left and down one tile
			if (tpx >= s->width - 1)
			{
				tpx -= s->width;
				tpy += 4;
			}
		}

		DCFlushRange(tex->img_ptr, s->width * s->height * 2);
		GX_InitTexObj((GXTexObj*)(tex->texobj), tex->img_ptr, s->width, s->height, GX_TF_RGB5A3, GX_CLAMP, GX_CLAMP, GX_FALSE);
		GX_InitTexObjLOD((GXTexObj*)(tex->texobj), GX_NEAR, GX_NEAR_MIP_NEAR, 0, 0, 0, GX_FALSE, GX_FALSE, GX_ANISO_1); //Nearest neighbor scaling
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
			GXTexture* tex = (GXTexture*)s->data;

			if (tex->texobj)
			{
				free(tex->texobj);
				tex->texobj = nullptr;
			}

			if (tex->img_ptr)
			{
				free(tex->img_ptr);
				tex->img_ptr = nullptr;
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

	x1 *= scale;
	y1 *= scale;
	x2 *= scale;
	y2 *= scale;

	currentTexture = nullptr;

	c.r &= 0xF8;
	c.g &= 0xF8;
	c.b &= 0xF8;

	GX_InvVtxCache();

	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4); 	// Draw A Quad
		GX_Position2f32(x1, y1);		// Top Left
		GX_Color3u8(c.r, c.g, c.b);
		
		GX_Position2f32(x2, y1);		// Top Right
		GX_Color3u8(c.r, c.g, c.b);
		
		GX_Position2f32(x2, y2);		// Bottom Right
		GX_Color3u8(c.r, c.g, c.b);
		
		GX_Position2f32(x1, y2);		// Bottom Left
		GX_Color3u8(c.r, c.g, c.b);
	GX_End();
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

	GXTexture* tex = (GXTexture*)s->data;

	if (currentTexture != tex)
	{
		currentTexture = tex;

		GX_LoadTexObj( (GXTexObj*)tex->texobj, GX_TEXMAP0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	}

	y -= scroll_y;

	f32 x1 = x * scale;
	f32 x2 = (x + cropw) * scale;

	f32 y1 = y * scale;
	f32 y2 = (y + croph) * scale;

	f32 cx1 = ((f32)cropx / (f32)s->width)  + (0.001f / (f32)s->width);
	f32 cy1 = ((f32)cropy / (f32)s->height) + (0.001f / (f32)s->height);
	f32 cx2 = ((f32)(cropx + cropw) / (f32)s->width)  - (0.001f / (f32)s->width);
	f32 cy2 = ((f32)(cropy + croph) / (f32)s->height) - (0.001f / (f32)s->height);


	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);	// Draw A Quad
		GX_Position2f32(x1, y1);		// Top Left
		GX_TexCoord2f32(cx1, cy1);

		GX_Position2f32(x2, y1);		// Top Right
		GX_TexCoord2f32(cx2, cy1);

		GX_Position2f32(x2, y2);		// Bottom Right
		GX_TexCoord2f32(cx2, cy2);

		GX_Position2f32(x1, y2);		// Bottom Left
		GX_TexCoord2f32(cx1, cy2);
	GX_End();
}

typedef struct
{
	unsigned short data;
	f32 cropx1;
	f32 cropy1;
	f32 cropx2;
	f32 cropy2;
} Tile;

//
Tilemap* gfx_LoadTilemap(Surface* s, int width, int height, unsigned char* data)
{
	if (!s) return nullptr;
	if (!s->data) return nullptr;

	Tilemap* tm = (Tilemap*)memalign(32, sizeof(Tilemap));

	tm->width = width;
	tm->height = height;
	tm->image = s;

	tm->data = (unsigned short*)memalign(32, sizeof(Tile) * width * height);
	Tile* t = (Tile*)tm->data;

	int i = 0;
	unsigned short tile;
	int cropx;
	int cropy;
	int cropw = 20;
	int croph = 20;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			tile = data[x+y*width];

			cropx = (tile * 20) % s->width;
			cropy = ((tile * 20) / s->width) * 20;

			t[i].data = tile;
			t[i].cropx1 = ((f32)cropx / (f32)s->width)  + (0.001f / (f32)s->width);
			t[i].cropy1 = ((f32)cropy / (f32)s->height) + (0.001f / (f32)s->height);
			t[i].cropx2 = ((f32)(cropx + cropw) / (f32)s->width)  - (0.001f / (f32)s->width);
			t[i].cropy2 = ((f32)(cropy + croph) / (f32)s->height) - (0.001f / (f32)s->height);

			i += 1;
		}
	}

	return tm;
/*
	tm->data = (unsigned short*)memalign(32, width * height * sizeof(unsigned short));
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
*/
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

	x -= 60;
	y -= 60;

	Surface* s = tm->image;
	GXTexture* tex = (GXTexture*)s->data;

	currentTexture = nullptr;

	GX_LoadTexObj( (GXTexObj*)tex->texobj, GX_TEXMAP0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	Tile* t;

	f32 x1, x2;
	f32 y1, y2;

	for (int ty = 0+3; ty < tm->height-3; ty++)
	{
		for (int tx = 0+3; tx < tm->width-3; tx++)
		{
			t = &((Tile*)tm->data)[tx + ty * 22];

			if (t->data == 0)
				continue;

			x1 = (x + tx*20) * scale;
			x2 = (x + tx*20 + 20) * scale;

			y1 = (y + ty*20) * scale;
			y2 = (y + ty*20 + 20) * scale;

			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);	// Draw A Quad
				GX_Position2f32(x1, y1);		// Top Left
				GX_TexCoord2f32(t->cropx1, t->cropy1);

				GX_Position2f32(x2, y1);		// Top Right
				GX_TexCoord2f32(t->cropx2, t->cropy1);

				GX_Position2f32(x2, y2);		// Bottom Right
				GX_TexCoord2f32(t->cropx2, t->cropy2);

				GX_Position2f32(x1, y2);		// Bottom Left
				GX_TexCoord2f32(t->cropx1, t->cropy2);
			GX_End();

			/*
			tile = ((unsigned short*)tm->data)[tx + ty * 22];
			if (tile != 0)
				gfx_DrawTile(tm->image, tx*20+x, ty*20+y, 20, 20, tile);
			*/
		}
	}
}