#include "secretcontroller.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../text.hpp"
#include <stdio.h>

SecretController::SecretController(int flag, int count, int* flags)
:Object(0,0)
{
	numOfFlags = count;
	check = false;

	for (int i = 0; i < 20; i++)
		this->flags[i] = -1;

	for (int i = 0; i < numOfFlags; i++)
		this->flags[i] = flags[i];

	if (gameGetFlag(flag))
		dead = true;
}

void SecretController::update()
{
	//destroy if secret is already found
	if (gameGetFlag(killFlag))
	{
		dead = true;
		return;
	}

	//check if there is an object waiting for a secret
	if (!check)
	{
		check = true;
		bool found = false;

		//check objects
		Object* ptr = objectList.next(nullptr);
		while (ptr && !found)
		{
			if (!ptr->isDead() && (ptr->getSpawnFlag() == killFlag || ptr->getDespawnFlag() == killFlag))
			{
				found = true;
				break;
			}
			ptr = objectList.next(ptr);
		}

		//check platforms
		ptr = platformList.next(nullptr);
		while (ptr && !found)
		{
			if (!ptr->isDead() && (ptr->getSpawnFlag() == killFlag || ptr->getDespawnFlag() == killFlag))
			{
				found = true;
				break;
			}
			ptr = platformList.next(ptr);
		}

		if (!found)
		{
			dead = true;
			return;
		}
	}

	//wait for all flags to be set
	bool fail = false;

	for (int i = 0; i < numOfFlags; i++)
	{
		if (flags[i] == -1)
			break;

		if (!gameGetFlag(flags[i]))
		{
			fail = true;
			break;
		}
	}

	//all flags are set, play jingle
	if (fail == false)
	{
		gamePlaySecret();
		dead = true;			
	}
}

void SecretController::draw(float subFrame, float depth)
{
	/*
	for (int i = 0; i < numOfFlags; i++)
	{
		char str[64];
		sprintf(str, "%d", flags[i]);
		drawTextBold(str, 10, i*20);
	}
	*/
}