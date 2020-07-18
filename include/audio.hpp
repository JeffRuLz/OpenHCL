#pragma once

typedef struct
{
	void* data;
} Sound;

typedef struct
{
	void* data;
} Music;

//
int aud_Init();

//
void aud_Exit();

int aud_GetMusicVolume();
void aud_SetMusicVolume(int n);

int aud_GetSoundVolume();
void aud_SetSoundVolume(int n);

Sound* aud_LoadSound(const char* fname);
Music* aud_LoadMusic(const char* fname);

void aud_FreeSound(Sound* s);
void aud_FreeMusic(Music* m);

void aud_PlaySound(Sound* s, float pan = 0.f);
void aud_PlayMusic(Music* m, bool loop);

void aud_StopAll();
void aud_StopMusic();
void aud_MuteMusic(bool mute);