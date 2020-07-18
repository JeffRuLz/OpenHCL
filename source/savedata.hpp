#pragma once

#pragma pack(push,1)
typedef struct
{
	unsigned long health;
	unsigned long maxHealth;
	unsigned long ammo;
	unsigned long maxAmmo;
	unsigned long weapon;
	bool flags[4500];
	unsigned long unknown; //Blank
	unsigned long playTime; //In frames
	unsigned long heroX;
	unsigned long heroY;
	unsigned long level;
	unsigned long screen;
	unsigned long heroDirection;
} SaveData;
#pragma pack(push,0)

SaveData* createSaveData();
void freeSaveData(SaveData* sd);

void updateSaveData(SaveData* sd);
void applySaveData(SaveData* sd);