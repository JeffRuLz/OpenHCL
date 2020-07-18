#include "savedata.hpp"
#include "hero.hpp"
#include "game.hpp"
#include <malloc.h>

SaveData* createSaveData()
{
	SaveData* sd = (SaveData*)malloc(sizeof(SaveData));

	sd->health = 128;
	sd->maxHealth = 128;
	sd->ammo = 0;
	sd->maxAmmo = 99;
	sd->weapon = 0;

	for (int i = 0; i < 4500; i++)
		sd->flags[i] = false;

	sd->unknown = 0;
	sd->playTime = 0;
	sd->heroX = 128;
	sd->heroY = 128;
	sd->level = 0;
	sd->screen = 28;
	sd->heroDirection = 0;

	return sd;
}

void freeSaveData(SaveData* sd)
{
	if (sd)
		free(sd);
}

void updateSaveData(SaveData* sd)
{
	if (!sd) return;

	sd->health = hero.getHp();
	sd->maxHealth = hero.getMaxHp();
	sd->ammo = hero.getAmmo();
	sd->maxAmmo = hero.getMaxAmmo();
	sd->weapon = hero.getWeapon();

	for (int i = 0; i < 4500; i++)
		sd->flags[i] = gameGetFlag(i);

	sd->unknown = 0;
	sd->playTime = gameGetPlayTime();
	sd->heroX = hero.getX() * 2;
	sd->heroY = hero.getY() * 2;
	sd->level = gameGetLevel();
	sd->screen = gameGetScreen();
	sd->heroDirection = (hero.getDirection() == -1);
}

void applySaveData(SaveData* sd)
{
	if (!sd) return;

	hero.setHp(sd->health);
	hero.setMaxHp(sd->maxHealth);
	hero.setAmmo(sd->ammo);
	hero.setMaxAmmo(sd->maxAmmo);

	for (int i = 0; i < 4500; i++)
		gameSetFlag(i, sd->flags[i]);

	hero.setWeapon(sd->weapon);
	gameSetPlayTime(sd->playTime);
	hero.setX(sd->heroX/2);
	hero.setY(sd->heroY/2);
	gameSetLevel(sd->level);
	gameSetScreen(sd->screen);
	hero.setDirection( (sd->heroDirection == 0)? 1: -1 );
}