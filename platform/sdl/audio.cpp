#include "audio.hpp"
#include "system.hpp"
#include <stdio.h>
#include <malloc.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

static int musicVol = 50;
static int soundVol = 50;

static int lastChan = 0;

int aud_Init()
{
	int ret = SDL_InitSubSystem(SDL_INIT_AUDIO);

	if (ret < 0)
	{
		printf("SDL Audio could not initialize! SDL_Error: %s\n", SDL_GetError());
		return ret;
	}

	Mix_Init(0);
	ret = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);

	if (ret < 0)
	{
		printf("SDL Mixer could not initialize! Mixer Error: %s\n", Mix_GetError());
		return ret;
	}

	return 0;
}

void aud_Exit()
{
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int aud_GetMusicVolume()
{
	return musicVol;
}

void aud_SetMusicVolume(int n)
{
	if (n < 0) n = 0;
	if (n > 100) n = 100;

	musicVol = n;

	Mix_VolumeMusic(MIX_MAX_VOLUME * (musicVol / 100.f));
}

int aud_GetSoundVolume()
{
	return soundVol;
}

void aud_SetSoundVolume(int n)
{
	if (n < 0) n = 0;
	if (n > 100) n = 100;

	soundVol = n;
}

Sound* aud_LoadSound(const char* fname)
{
	char fpath[256];
	sprintf(fpath, "%swav/%s", SYS_DATAPATH, fname);

	Sound* s = (Sound*)malloc(sizeof(Sound));
	s->data = (void*)Mix_LoadWAV(fpath);

	#ifdef _DEBUG
	if (!s->data)
		printf("aud_LoadSound() could not load %s\n", fpath);
	#endif

	return s;
}

Music* aud_LoadMusic(const char* fname)
{
	char fpath[128];
	sprintf(fpath, "%sogg/%s.ogg", SYS_DATAPATH, fname);

	Music* m = (Music*)malloc(sizeof(Music));
	m->data = (void*)Mix_LoadMUS(fpath);

	#ifdef _DEBUG
	if (!m->data)		
		printf("Mix_LoadMUS error - %s", Mix_GetError());
	#endif

	return m;
}

void aud_FreeSound(Sound* s)
{
	if (s)
	{
		if (s->data)
		{
			Mix_FreeChunk((Mix_Chunk*)s->data);
			s->data = nullptr;
		}

		free(s);
	}
}

void aud_FreeMusic(Music* m)
{
	if (m)
	{
		if (m->data)
		{
			Mix_FreeMusic((Mix_Music*)m->data);
			m->data = nullptr;
		}

		free(m);
	}
}

void aud_PlaySound(Sound* s, float pan)
{
	if (!s)
		return;

	if (!s->data)
		return;

	if ((lastChan += 1) >= MIX_CHANNELS)
		lastChan = 1;

	Mix_HaltChannel(lastChan);
	int channel = Mix_PlayChannel(lastChan, (Mix_Chunk*)s->data, 0);

	if (channel != -1)
	{
		//volume
		int vol = MIX_MAX_VOLUME * (soundVol / 100.f);
		Mix_Volume(channel, vol);

		//panning
		if (pan > 1.f) pan = 1.f;
		if (pan < -1.f) pan = -1.f;

		unsigned char panLeft = 127.f + (127.f * -pan);
		unsigned char panRight = 127.f + (127.f * pan);

		Mix_SetPanning(channel, panLeft, panRight);
	}
}

void aud_PlayMusic(Music* m, bool loop)
{
	Mix_HaltMusic();

	if (!m) return;
	if (!m->data) return;

	if (Mix_PlayMusic( (Mix_Music*)m->data, ((loop)? -1: 0) ) == 0)
	{
		Mix_VolumeMusic(MIX_MAX_VOLUME * (musicVol / 100.f));
	}
}

void aud_StopAll()
{
	aud_StopMusic();

	for (int i = 0; i < MIX_CHANNELS; i++)
		Mix_HaltChannel(i);
}

void aud_StopMusic()
{
	Mix_HaltMusic();
}

void aud_MuteMusic(bool mute)
{
	int vol = MIX_MAX_VOLUME * (musicVol / 100.f);

	if (mute)
		vol = 0;

	Mix_VolumeMusic(vol);
}