#include "audio.hpp"
#include "system.hpp"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

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
	u32 sampleRate;
	u32 size;
	u16 bitsPerSample;
	u16 numChannels;
	u16 format;
	u8* data;
} SoundData;

typedef struct
{
	u32 size;
	u8* data;
} MusicData;

static char musicVol = 80;
static char soundVol = 80;

#define NDSP_NUM_CHANNELS 24
static ndspWaveBuf waveBuf[NDSP_NUM_CHANNELS];

#define BUF_SIZE (4096)
static int fillBlock = 0;
static bool musicIsPlaying = false;
static bool musicLoop = false;
static u16 musicChannels = 2;

static OggVorbis_File vf;
static int current_section = 0;

static void musicCallback(void* data);

int aud_Init()
{
	int ret = ndspInit();

	if (ret)
	{
		#ifdef _DEBUG
		printf("ndspInit failed.\nDid you dump your dspfirm.cdc?\n");
		#endif
	}

	waveBuf[0].data_vaddr = (char*)linearAlloc(BUF_SIZE);
	waveBuf[1].data_vaddr = (char*)linearAlloc(BUF_SIZE);

	ndspSetCallback(musicCallback, 0);

	return ret;
}

void aud_Exit()
{
	aud_StopMusic();

	linearFree((char*)waveBuf[0].data_vaddr);
	linearFree((char*)waveBuf[1].data_vaddr);

	ndspExit();
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

	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0 * (float)musicVol / 100.f;
	mix[1] = 1.0 * (float)musicVol / 100.f;
	ndspChnSetMix(0, mix);
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
	Sound* s = nullptr;

	char fpath[128];
	sprintf(fpath, "%swav/%s", SYS_DATAPATH, fname);

	FILE* f = fopen(fpath, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("aud_LoadSound - could not load %s\n", fpath);
		#endif
	}
	else
	{
		s = (Sound*)malloc(sizeof(Sound));

		WAVHeader wh;
		fread(&wh, 1, sizeof(WAVHeader), f);

		s->data = (SoundData*)malloc(sizeof(SoundData));
		SoundData* snd = (SoundData*)s->data;

		if (wh.BitsPerSample == 8)
		{
			snd->format = (wh.NumChannels == 1)?
				NDSP_FORMAT_MONO_PCM8:
				NDSP_FORMAT_STEREO_PCM8;
		}
		else if (wh.BitsPerSample == 16)
		{
			snd->format = (wh.NumChannels == 1)?
				NDSP_FORMAT_MONO_PCM16:
				NDSP_FORMAT_STEREO_PCM16;
		}

		snd->sampleRate = (float)wh.SampleRate;
		snd->size = wh.Subchunk2Size;
		snd->bitsPerSample = wh.BitsPerSample;
		snd->numChannels = wh.NumChannels;

		snd->data = (u8*)linearAlloc(snd->size);
		fread(snd->data, 1, snd->size, f);

		//convert 8-bit unsigned to 8-bit signed
		if (snd->bitsPerSample == 8)
		{
			for (u32 i = 0; i < snd->size; i++)
				snd->data[i] -= 128;
		}
	}

	fclose(f);

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

		md->data = (u8*)linearAlloc(md->size);
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
				linearFree(snd->data);
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
				linearFree(md->data);
				md->data = nullptr;
			}

			free(m->data);
			m->data = nullptr;
		}

		free(m);
	}
}

void aud_PlaySound(Sound* s, float pan)
{
	if (!s) return;
	if (!s->data) return;

	SoundData* snd = (SoundData*)s->data;

	//search for first free channel
	int chan;
	for (chan = 2; chan < NDSP_NUM_CHANNELS; chan++)
	{
		if (!ndspChnIsPlaying(chan))
			break;
	}

	//prepare channel
	ndspChnReset(chan);			
	ndspChnSetInterp(chan, NDSP_INTERP_NONE);
	ndspChnSetRate(chan, (float)snd->sampleRate);
	ndspChnSetFormat(chan, snd->format);

	//set volume
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0 * (float)soundVol / 100.f;
	mix[1] = 1.0 * (float)soundVol / 100.f;
	ndspChnSetMix(chan, mix);

	//setup buffer
	memset(&waveBuf[chan], 0, sizeof(ndspWaveBuf));
	waveBuf[chan].data_vaddr = (u32*)snd->data;
	waveBuf[chan].nsamples = snd->size / (snd->bitsPerSample >> 3) / snd->numChannels;
	waveBuf[chan].looping = false;
	waveBuf[chan].status = NDSP_WBUF_FREE;

	//flush data
	DSP_FlushDataCache(snd->data, snd->size);		
	//play sound
	ndspChnWaveBufAdd(chan, &waveBuf[chan]);
}

static void musicCallback(void* data)
{
	if (!musicIsPlaying)
		return;

	if (waveBuf[fillBlock].status == NDSP_WBUF_DONE)
	{
		long inPos = 0;

		while (inPos < BUF_SIZE)
		{
			long size = BUF_SIZE - inPos;
			long ret = ov_read(&vf, (char*)waveBuf[fillBlock].data_vaddr + inPos, size, &current_section);

			if (ret == 0)
			{
				//EOF
				if (musicLoop)
				{
					current_section = 0;
					ov_raw_seek(&vf, current_section);
					continue;
				}
				else
				{
					aud_StopMusic();
					break;
				}
			}
			else if (ret < 0)
			{
				break;
			}
			else
			{
				inPos += ret;
			}
		}

		waveBuf[fillBlock].status = NDSP_WBUF_FREE;

		DSP_FlushDataCache(waveBuf[fillBlock].data_vaddr, BUF_SIZE);
		ndspChnWaveBufAdd(0, &waveBuf[fillBlock]);

		fillBlock = !fillBlock;
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
		musicIsPlaying = true;
		musicLoop = loop;

		vorbis_info* vi = ov_info(&vf, -1);
		musicChannels = vi->channels;
		u16 format = (musicChannels == 1)?
			NDSP_FORMAT_MONO_PCM16:
			NDSP_FORMAT_STEREO_PCM16;

		//prepare channel
		ndspChnReset(0);			
		ndspChnSetInterp(0, NDSP_INTERP_NONE);
		ndspChnSetRate(0, (float)vi->rate);
		ndspChnSetFormat(0, format);

		float mix[12];
		memset(mix, 0, sizeof(mix));
		mix[0] = 1.0 * (float)musicVol / 100.f;
		mix[1] = 1.0 * (float)musicVol / 100.f;
		ndspChnSetMix(0, mix);

		//setup buffer
		waveBuf[0].nsamples = 
		waveBuf[1].nsamples = BUF_SIZE / 2 / musicChannels;
		waveBuf[0].looping = 
		waveBuf[1].looping = false;
		waveBuf[0].status = 
		waveBuf[1].status = NDSP_WBUF_DONE;
	}	
}

void aud_StopAll()
{
	aud_StopMusic();

	for (int i = 0; i < NDSP_NUM_CHANNELS; i++)
		ndspChnWaveBufClear(i);
}

void aud_StopMusic()
{
	musicIsPlaying = false;
	ov_clear(&vf);
}

void aud_MuteMusic(bool mute)
{
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0 * (float)musicVol / 100.f;
	mix[1] = 1.0 * (float)musicVol / 100.f;

	if (mute)
	{
		mix[0] = 0;
		mix[1] = 0;
	}

	ndspChnSetMix(0, mix);
}