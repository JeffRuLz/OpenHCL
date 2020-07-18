#include "chest.hpp"
#include "input.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../effects/sparkle.hpp"

Chest::Chest(float x, float y, bool isKey, int flag)
:Object(x,y)
{
	this->isKey = isKey;
	this->flag = flag;
	timer = 0;

	if (this->flag == 0 || gameGetFlag(this->flag))
		dead = true;
}

void Chest::create()
{
	if (spawnFlag != 0 && !gameGetFlag(spawnFlag))
		gameRingBell();
}

void Chest::update()
{
	if (gameGetState() == GAME_STATE_SAVE)
		return;

	//sparkle
	if (timer++ > 12)
	{
		timer = 0;
		effectList.add(new Sparkle(x + (rand() % 20), y + (rand() % 20)));
	}

	//open
	if (inp_ButtonPressed(BTN_UP) && hero.isOnGround())
	{
		Rectangle mask = (Rectangle) { (int)x+2, (int)y+5, 16, 15 };

		if (collision(hero.getX(), hero.getY(), mask))
		{
			aud_PlaySound(sounds[sndGet02]);
			gameSetLastItem(flag);
			hero.setState(HERO_STATE_ITEM_GET);
			dead = true;
		}
	}
}

void Chest::draw(float subFrame, float depth)
{
	int tile[] = { 13, 54 };
	gfx_DrawTile(images[imgChr20], x, y, 20, 20, tile[isKey]);
}