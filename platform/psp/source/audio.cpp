#include "audio.hpp"
#include "system.hpp"
#include "callbacks.h"
#include "../../../source/game.hpp"

#include <psptypes.h>
#include <pspaudio.h>
#include <pspaudiolib.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct
{
	u32 sampleRate;
	u32 size;
	u16 bitsPerSample;
	u16 numChannels;
	PspAudioFormats format;
	short* data;
} SoundData;

typedef struct
{
	u32 size;
	u8* data;
} MusicData;

#pragma pack(push,1)
typedef struct
{
	unsigned int ChunkID;
	unsigned int ChunkSize;
	unsigned int Format;
	unsigned int Subchunk1ID;
	unsigned int Subchunk1Size;
	unsigned short AudioFormat;
	unsigned short NumChannels;
	unsigned int SampleRate;
	unsigned int ByteRate;
	unsigned short BlockAlign;
	unsigned short BitsPerSample;
	unsigned int Subchunk2ID;
	unsigned int Subchunk2Size;
} WAVHeader;
#pragma pack(push,0)

typedef struct
{
	bool playing;
	bool loop;
	unsigned int pos;
	void* data;
} SoundChannel;

typedef struct {
    short l, r;
} sample_t;

static char musicVolume = 80;
static char soundVolume = 80;

static int lastChan = 0;

static SoundChannel channel[PSP_NUM_AUDIO_CHANNELS];

#define BUF_SIZE 4096
static char pcmout[BUF_SIZE];
static OggVorbis_File vf;
static int current_section = 0;

static int bufPtr = 0;
static int bufLen = 0;

static Music* lastMusic = nullptr;
static bool musicInterupt = false;

static void audioCallback(void* buf, unsigned int length, void* userdata);
static void musicCallback(void* buf, unsigned int length, void* userdata);

int aud_Init()
{
	for (int i = 0; i < PSP_NUM_AUDIO_CHANNELS; i++)
	{
		channel[i].playing = false;
		channel[i].loop = false;
		channel[i].pos = 0;
		channel[i].data = nullptr;
	}

	pspAudioInit();

	pspAudioSetChannelCallback(0, musicCallback, (void*)&(channel[0]));
	pspAudioSetChannelCallback(1, audioCallback, (void*)&(channel[1]));
	pspAudioSetChannelCallback(2, audioCallback, (void*)&(channel[2]));
	pspAudioSetChannelCallback(3, audioCallback, (void*)&(channel[3]));

	return 0;
}

void aud_Exit()
{
	aud_StopMusic();
	pspAudioEnd();
}

int aud_GetMusicVolume()
{
	return musicVolume;
}

void aud_SetMusicVolume(int n)
{
	if (n < 0) n = 0;
	if (n > 100) n = 100;

	musicVolume = n;

	unsigned int vol = PSP_VOLUME_MAX * (musicVolume / 100.f);
	pspAudioSetVolume(0, vol, vol);
}

int aud_GetSoundVolume()
{
	return soundVolume;
}

void aud_SetSoundVolume(int n)
{
	if (n < 0) n = 0;
	if (n > 100) n = 100;

	soundVolume = n;
}

Sound* aud_LoadSound(const char* fname)
{
	char fpath[128];
	sprintf(fpath, "%swav/%s", SYS_DATAPATH, fname);

	Sound* s = nullptr;
	FILE* f = fopen(fpath, "rb");

	if (f)
	{
		s = (Sound*)malloc(sizeof(Sound));

		WAVHeader wh;
		fread(&wh, 1, sizeof(WAVHeader), f);

		s->data = (SoundData*)malloc(sizeof(SoundData));
		SoundData* snd = (SoundData*)s->data;

		snd->format = (wh.NumChannels == 1)?
			PSP_AUDIO_FORMAT_MONO:
			PSP_AUDIO_FORMAT_STEREO;

		snd->sampleRate = (float)wh.SampleRate;
		snd->size = wh.Subchunk2Size / sizeof(short);
		snd->bitsPerSample = wh.BitsPerSample;
		snd->numChannels = wh.NumChannels;

		snd->data = (short*)malloc(snd->size * sizeof(short));
		memset(snd->data, 0, snd->size * sizeof(short));
		fread(snd->data, sizeof(short), snd->size, f);

		fclose(f);
	}

	return s;
}

Music* aud_LoadMusic(const char* fname)
{
	char str[128];
	sprintf(str, "%sogg/%s.ogg", SYS_DATAPATH, fname);
	FILE* f = fopen(str, "rb");

	if (f)
	{
		Music* m = (Music*)malloc(sizeof(Music));

		m->data = (MusicData*)malloc(sizeof(MusicData));
		MusicData* md = (MusicData*)m->data;

		fseek(f, 0, SEEK_END);
		md->size = ftell(f);

		md->data = (u8*)malloc(md->size);
		fseek(f, 0, SEEK_SET);
		fread(md->data, 1, md->size, f);

		fclose(f);
		return m;
	}

	return nullptr;
}

void aud_FreeSound(Sound* s)
{
	if (s)
	{
		if (s->data)
		{
			SoundData* snd = (SoundData*)s->data;

			if (snd->data)
			{
				free(snd->data);
				snd->data = nullptr;
			}

			free(s->data);
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
			MusicData* md = (MusicData*)m->data;

			if (md->data)
			{
				free(md->data);
				md->data = nullptr;
			}

			free(m->data);
			m->data = nullptr;
		}

		free(m);
	}
}

static void audioCallback(void* buf, unsigned int length, void* userdata)
{
	SoundChannel* chn = (SoundChannel*)userdata;
	sample_t* ubuf = (sample_t*)buf;

	if (chn->playing == false)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			ubuf[i].l = 0;
			ubuf[i].r = 0;
		}

		return;
	}

	SoundData* snd = (SoundData*)chn->data;

	for (unsigned int i = 0; i < length; i++)
	{
		unsigned int p = i + chn->pos;

		if (p < snd->size)
		{
			ubuf[i].l = snd->data[p];
			ubuf[i].r = snd->data[p];
		}
		else
		{
			chn->playing = false;

			ubuf[i].l = 0;
			ubuf[i].r = 0;
		}
	}

	chn->pos += length;		
}

void aud_PlaySound(Sound* s, float pan)
{
	if (!s) return;
	if (!s->data) return;

	int chan = -1;

	//normally: new sound will play on the oldest channel
	//if the secret jingle is playing: only play if a channel is free
	//hacky solution, but it works
	if (gameGetSecretTimer() <= 0)
	{
		if ((lastChan += 1) >= PSP_NUM_AUDIO_CHANNELS)
			lastChan = 1;

		chan = lastChan;
	}
	else
	{
		for (int i = 1; i < PSP_NUM_AUDIO_CHANNELS; i++)
		{
			if (channel[i].playing == false)
			{
				chan = i;
				break;
			}
		}
	}

	//
	if (chan >= 1)
	{
		channel[chan].playing = true;
		channel[chan].loop = false;
		channel[chan].pos = 0;		
		channel[chan].data = (void*)s->data;

		unsigned int vol = PSP_VOLUME_MAX * (soundVolume / 100.f);
		pspAudioSetVolume(chan, vol, vol);
	}
}

static void musicCallback(void* buf, unsigned int length, void* userdata)
{
	SoundChannel* chn = (SoundChannel*)userdata;
	sample_t* ubuf = (sample_t*)buf;

	if (!chn->playing || musicInterupt)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			ubuf[i].l = 0;
			ubuf[i].r = 0;
		}

		return;
	}

	short* ptr = (short*)pcmout;

	for (unsigned int i = 0; i < length; i++)
	{
		if (bufPtr >= bufLen)
		{
			bufPtr = 0;

			long ret = ov_read(&vf, pcmout, length*2, &current_section);

			if (ret == 0)
			{
				//EOF
				if (chn->loop)
				{
					current_section = 0;
					ov_raw_seek(&vf, current_section);
				}
				else
				{
					aud_StopMusic();
				}
			}
			else if (ret < 0)
			{
				//error
			}
			else
			{
				bufLen = ret/2;
			}
		}		

		ubuf[i].l = ptr[bufPtr++];
		ubuf[i].r = ptr[bufPtr++];
	}
}

void aud_PlayMusic(Music* m, bool loop)
{
	if (!m) return;
	if (!m->data) return;

	aud_StopMusic();

	MusicData* md = (MusicData*)m->data;

	if (ov_open(fmemopen(md->data, md->size, "rb"), &vf, NULL, 0) >= 0)
	{
		musicInterupt = false;
		lastMusic = m;

		current_section = 0;

		channel[0].playing = true;
		channel[0].loop = loop;
		channel[0].pos = 0;		
		channel[0].data = nullptr;

		unsigned int vol = PSP_VOLUME_MAX * (musicVolume / 100.f);
		pspAudioSetVolume(0, vol, vol);
	}
}

void aud_StopAll()
{
	aud_StopMusic();

	for (int i = 0; i < PSP_NUM_AUDIO_CHANNELS; i++)
	{
		channel[i].playing = false;
	}
}

void aud_StopMusic()
{
	lastMusic = nullptr;
	channel[0].playing = false;
	ov_clear(&vf);
}

void aud_MuteMusic(bool mute)
{
	unsigned int vol = PSP_VOLUME_MAX * (musicVolume / 100.f);

	if (mute)
		vol = 0;

	pspAudioSetVolume(0, vol, vol);
}