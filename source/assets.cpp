#include "assets.hpp"
#include "system.hpp"
#include "QDArc.hpp"

Surface* images[NUM_OF_IMAGES] = { nullptr };
Sound* sounds[NUM_OF_SOUNDS] = { nullptr };
Music* music[NUM_OF_MUSIC] = { nullptr };

void loadTitleAssets()
{
	//graphics
	{
		char qdaPath[128];
		sprintf(qdaPath, "%sbmp.qda", SYS_DATAPATH);

		if (images[imgTitle01] == nullptr)
		{
			FILE* f = ExtractFromQDAFile(qdaPath, "title01.BMP");

			if (f)
				images[imgTitle01] = gfx_LoadSurface(f);
			fclose(f);
		}

		if (images[imgFont] == nullptr)
		{
			FILE* f = ExtractFromQDAFile(qdaPath, "font8x8-01.bmp");

			if (f)
				images[imgFont] = gfx_LoadSurface(f);
			fclose(f);
		}

		if (images[imgFontKana] == nullptr)
		{
			FILE* f = ExtractFromQDAFile(qdaPath, "font8x8-kana.bmp");

			if (f)
				images[imgFontKana] = gfx_LoadSurface(f);
			fclose(f);
		}
	}

	//sounds
	if (sounds[sndPi01] == nullptr)
		sounds[sndPi01] = aud_LoadSound("pi01.wav");

	if (sounds[sndOk] == nullptr)
		sounds[sndOk] = aud_LoadSound("ok.wav");
}

void loadAssets()
{
	//load graphics
	{
		const char* fname[] = {
			"chr20.BMP",
			"chr20x40.BMP",
			"chr32.BMP",
			"chr60x20.BMP",
			"chr64.BMP",
			"dark.bmp",
			"ene01.bmp",
			"font8x8-01.bmp",
			"font8x8-kana.bmp",
			"items.bmp",
			"mychr.bmp",
			"status.bmp",
			"title01.BMP",
			"stage01.bmp",
			"boss01.bmp"
		};
		
		char qdaPath[128];
		sprintf(qdaPath, "%sbmp.qda", SYS_DATAPATH);

		for (int i = 0; i < NUM_OF_IMAGES; i++)
		{
			if (images[i] == nullptr)
			{
				FILE* f = ExtractFromQDAFile(qdaPath, fname[i]);

				if (f)
					images[i] = gfx_LoadSurface(f);
				fclose(f);
			}
		}
	}

	//load sounds
	{
		const char* fname[] = {
			"bee01.wav",
			"bell01.wav",
			"bom01.wav",
			"bom02.wav",
			"bom03.wav",
			"door00.wav",
			"fire01.wav",
			"gas01.wav",
			"get01.wav",
			"get02.wav",
			"hit01.wav",
			"hit02.wav",
			"hit03.wav",
			"hit04.wav",
			"hit05.wav",
			"hit06.wav",
			"hit07.wav",
			"jump01.wav",
			"jump02.wav",
			"nazo.wav",
			"ng.wav",
			"ok.wav",
			"pi01.wav",
			"pi02.wav",
			"pi03.wav",
			"pi04.wav",
			"pi05.wav",
			"pi06.wav",
			"pi07.wav",
			"pi08.wav",
			"pi09.wav",
			"pi10.wav",
			"power01.wav",
			"power02.wav",
			"shot01.wav",
			"shot02.wav",
			"shot03.wav",
			"shot04.wav",
			"shot05.wav",
			"shot06.wav",
			"shot07.wav",
			"step01.wav",
			"water01.wav",
			"wolf01.wav"
		};

		for (int i = 0; i < NUM_OF_SOUNDS; i++)
		{
			if (sounds[i] == nullptr)
				sounds[i] = aud_LoadSound(fname[i]);
		}
	}

	//load music
	{
		const char* fname[] = {
			"main01",
			"boss",
			"nazo",
			"gameover",
			"start",
			"allclear"
		};

		for (int i = 0; i < NUM_OF_MUSIC; i++)
		{
			if (music[i] == nullptr)
				music[i] = aud_LoadMusic(fname[i]);
		}
	}
}

void freeAssets()
{
	//free graphics
	for (int i = 0; i < NUM_OF_IMAGES; i++)
	{
		if (images[i] != nullptr)
		{
			gfx_FreeSurface(images[i]);
			images[i] = nullptr;
		}
	}

	//free sounds
	for (int i = 0; i < NUM_OF_SOUNDS; i++)
	{
		if (sounds[i] != nullptr)
		{
			aud_FreeSound(sounds[i]);
			sounds[i] = nullptr;
		}
	}

	//free music
	for (int i = 0; i < NUM_OF_MUSIC; i++)
	{
		if (music[i] != nullptr)
		{
			aud_FreeMusic(music[i]);
			music[i] = nullptr;
		}
	}
}

void loadTileset(int level)
{
	if (level < 0 || level > 8) level = 0;

	const char* fname[] = {
		"stage01.bmp",
		"stage02.bmp",
		"stage02.bmp",
		"stage03.bmp",
		"stage04.bmp",
		"stage02.bmp",
		"stage03.bmp",
		"stage05.bmp",
		"stage08.bmp"
	};

	char qdaPath[128];
	sprintf(qdaPath, "%sbmp.qda", SYS_DATAPATH);

	FILE* f = ExtractFromQDAFile(qdaPath, fname[level]);

	if (f)
	{
		if (images[imgTiles] != nullptr)
		{
			gfx_FreeSurface(images[imgTiles]);
			images[imgTiles] = nullptr;
		}

		images[imgTiles] = gfx_LoadSurface(f);
		fclose(f);
	}
}

void loadBossGraphics(int level)
{
	if (level < 0 || level > 9) level = 0;

	const char* fname[] = {
		"boss01.bmp",
		"boss01.bmp",
		"boss03.bmp",
		"boss06.bmp",
		"boss02.bmp",
		"boss04.bmp",
		"boss05.bmp",
		"boss07.bmp",
		"boss01.bmp",
		"lboss01.bmp"
	};

	char qdaPath[128];
	sprintf(qdaPath, "%sbmp.qda", SYS_DATAPATH);

	FILE* f = ExtractFromQDAFile(qdaPath, fname[level]);

	if (f)
	{
		if (images[imgBoss] != nullptr)
		{
			gfx_FreeSurface(images[imgBoss]);
			images[imgBoss] = nullptr;
		}

		images[imgBoss] = gfx_LoadSurface(f);
		fclose(f);
	}
}

void loadMusic(int level)
{
	if (level < 0 || level > 8) level = 0;

	const char* fname[] = {
		"main01",
		"main02",
		"main02",
		"main05",
		"main03",
		"main02",
		"main05",
		"main04",
		"main06"
	};

	if (music[bgmMusic] != nullptr)
	{
		aud_FreeMusic(music[bgmMusic]);
		music[bgmMusic] = nullptr;
	}
	
	music[bgmMusic] = aud_LoadMusic(fname[level]);

	//boss music
	if (level == 8)
	{
		aud_FreeMusic(music[bgmBoss]);
		music[bgmBoss] = aud_LoadMusic("lastboss");
	}
	else if (level != 0)
	{
		aud_FreeMusic(music[bgmBoss]);
		music[bgmBoss] = aud_LoadMusic("boss");
	}
}