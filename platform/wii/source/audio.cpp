#include "audio.hpp"
#include "system.hpp"
#include <gccore.h>
#include <asndlib.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <ogc/machine/processor.h>

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
	s32 format;
	u8* data ATTRIBUTE_ALIGN(32);
} SoundData;

typedef struct
{
	u32 size;
	u8* data;
} MusicData;

static volatile int musicVol = 70;
static int soundVol = 70;

static int lastChan = 0;

static volatile bool runThread = false;
static volatile bool musicIsPlaying = false;
static volatile bool musicLoop = false;

static volatile OggVorbis_File vf;
static volatile int current_section = 0;

static volatile s32 musicFormat = 0;
static volatile s32 musicSampleRate = 0;

static volatile bool firstPlay = true;

static lwp_t thread;

static void* musicThread(void* arg);

int aud_Init()
{
	ASND_Init();
	ASND_Pause(0);

	runThread = true;
	musicIsPlaying = false;
	LWP_CreateThread(&thread, musicThread, NULL, NULL, 0, 80);

	return 0;
}

void aud_Exit()
{
	aud_StopAll();

	runThread = false;
	LWP_JoinThread(thread, NULL);

	ASND_End();
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
	int vol = 127.f * (musicVol / 100.f);
	ASND_ChangeVolumeVoice(0, vol, vol);
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

static void _swapWavHeader(WAVHeader* wh)
{
	wh->ChunkID 	  = bswap32(wh->ChunkID);
	wh->ChunkSize 	  = bswap32(wh->ChunkSize);
	wh->Format 		  = bswap32(wh->Format);
	wh->Subchunk1ID   = bswap32(wh->Subchunk1ID);
	wh->Subchunk1Size = bswap32(wh->Subchunk1Size);

	wh->AudioFormat = bswap16(wh->AudioFormat);
	wh->NumChannels = bswap16(wh->NumChannels);

	wh->SampleRate = bswap32(wh->SampleRate);
	wh->ByteRate   = bswap32(wh->ByteRate);

	wh->BlockAlign    = bswap16(wh->BlockAlign);
	wh->BitsPerSample = bswap16(wh->BitsPerSample);

	wh->Subchunk2ID   = bswap32(wh->Subchunk2ID);
	wh->Subchunk2Size = bswap32(wh->Subchunk2Size);
}

Sound* aud_LoadSound(const char* fname)
{
	Sound* s = nullptr;

	char fpath[128];
	sprintf(fpath, "%swav/%s", SYS_DATAPATH, fname);

	FILE* f = fopen(fpath, "rb");

	if (f)
	{
		s = (Sound*)memalign(32, sizeof(Sound));
		s->data = (SoundData*)memalign(32, sizeof(SoundData));
		SoundData* snd = (SoundData*)s->data;

		WAVHeader wh;
		fread(&wh, 1, sizeof(WAVHeader), f);
		_swapWavHeader(&wh);

		if (wh.BitsPerSample == 8)
		{
			snd->format = (wh.NumChannels == 1)?
				VOICE_MONO_8BIT:
				VOICE_STEREO_8BIT;
		}
		else if (wh.BitsPerSample == 16)
		{
			snd->format = (wh.NumChannels == 1)?
				VOICE_MONO_16BIT_LE:
				VOICE_STEREO_16BIT_LE;
		}

		snd->sampleRate = (float)wh.SampleRate;
		snd->size = ceil((float)((float)wh.Subchunk2Size / 32.f)) * 32;
		snd->bitsPerSample = wh.BitsPerSample;
		snd->numChannels = wh.NumChannels;

		snd->data = (u8*)memalign(32, snd->size);
		memset(snd->data, 0, snd->size);
		fread(snd->data, 1, wh.Subchunk2Size, f);
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
		Music* m = (Music*)memalign(32, sizeof(Music));
	
		m->data = (MusicData*)memalign(32, sizeof(MusicData));
		MusicData* md = (MusicData*)m->data;

		fseek(f, 0, SEEK_END);
		md->size = ftell(f);

		md->data = (u8*)memalign(32, md->size);
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

void aud_PlaySound(Sound* s, float pan)
{
	if (!s) return;
	if (!s->data) return;

	SoundData* snd = (SoundData*)s->data;

	if ((lastChan += 1) >= 16)
		lastChan = 1;

	s32 chan = lastChan;

	if (pan > 1.0) pan = 1.0;
	if (pan < -1.0) pan = -1.0;

	int left  = (127.f + (127.f * -pan)) * (soundVol / 100.f);
	int right = (127.f + (127.f *  pan)) * (soundVol / 100.f);

	ASND_SetVoice(chan, snd->format, snd->sampleRate, 0, snd->data, snd->size, left, right, NULL);
}

static volatile bool loadBuff = true;

static void musicCallback(int voice)
{
	loadBuff = true;
}

static void* musicThread(void* arg)
{
	#define BUF_SIZE (4096*2)

	char* pcmout[2];
	bool fillBlock = 0;

	pcmout[0] = (char*)memalign(32, BUF_SIZE);
	pcmout[1] = (char*)memalign(32, BUF_SIZE);

	while (runThread)
	{
		usleep(10000);

		if (!musicIsPlaying || !runThread || !loadBuff)
			continue;

		long inPos = 0;
		long size;
		long ret;

		while (inPos < BUF_SIZE)
		{
			size = BUF_SIZE - inPos;
			ret = ov_read((OggVorbis_File*)&vf, pcmout[fillBlock] + inPos, size, (int*)&current_section);

			if (ret == 0)
			{
				//EOF
				if (musicLoop)
				{
					current_section = 0;
					ov_raw_seek((OggVorbis_File*)&vf, current_section);
					continue;
				}
				else
				{
					musicIsPlaying = false;
					break;
				}
			}
			else if (ret < 0)
			{
				musicIsPlaying = false;
				break;
			}
			else
			{
				inPos += ret;
			}				
		}

		if (firstPlay)
		{
			firstPlay = false;
			int vol = 127.f * (musicVol / 100.f);
			ASND_SetVoice(0, musicFormat, musicSampleRate, 0, pcmout[fillBlock], BUF_SIZE, vol, vol, musicCallback);
		}
		else
		{
			ASND_AddVoice(0, pcmout[fillBlock], BUF_SIZE);
		}

		fillBlock = !fillBlock;
		loadBuff = false;
	}

	free(pcmout[1]);
	free(pcmout[0]);

	return 0;
}

void aud_PlayMusic(Music* m, bool loop)
{
	if (!m) return;
	if (!m->data) return;

	musicIsPlaying = false;
	ov_clear((OggVorbis_File*)&vf);

	MusicData* md = (MusicData*)m->data;

	if (ov_open(fmemopen(md->data, md->size, "rb"), (OggVorbis_File*)&vf, NULL, 0) >= 0)
	{
		current_section = 0;

		vorbis_info* vi = ov_info((OggVorbis_File*)&vf, -1);
		int numChannels = vi->channels;
		musicSampleRate = (float)vi->rate;
		musicFormat = (numChannels == 1)?
			VOICE_MONO_16BIT:
			VOICE_STEREO_16BIT;
		
		musicIsPlaying = true;
		musicLoop = loop;
		loadBuff = true;
		firstPlay = true;
	}
}

void aud_StopAll()
{
	aud_StopMusic();

	for (int i = 0; i < 16; i++)
		ASND_StopVoice(i);
}

void aud_StopMusic()
{
	ASND_StopVoice(0);
	musicIsPlaying = false;
	ov_clear((OggVorbis_File*)&vf);
}

void aud_MuteMusic(bool mute)
{
	int vol = 127.f * (musicVol / 100.f);
	
	if (mute)
		vol = 0;

	ASND_ChangeVolumeVoice(0, vol, vol);
}