#include "wizard.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_STAND,
	STATE_FLASH,
	STATE_INVISIBLE
};

static bool playSound = true;

Wizard::Wizard(float x, float y, int flag)
:Object(x,y)
{
	this->flag = flag;
	state = STATE_STAND;
	timer = 60;
	counter = 0;
	visible = true;
	imageIndex = 0;

	playSound = true;
}

void Wizard::update()
{
	int pattern[] = {
		STATE_STAND,
		STATE_FLASH,
		STATE_INVISIBLE,
		STATE_FLASH
	};

	if (timer > 0)
		timer -= 1;

	//animate
	imageIndex = fmod(imageIndex += 0.3, 3);

	switch (state)
	{
		case STATE_STAND:
		{
			visible = true;

			if (timer <= 0)
			{
				if (playSound)
				{
					aud_PlaySound(sounds[sndPi10]);
					playSound = false;
				}

				counter++;
				state = pattern[counter];
				timer = 15;
			}
		}
		break;

		case STATE_FLASH:
		{
			visible = !visible;

			if (timer <= 0)
			{
				if ((counter += 1) >= 4)
					counter -= 4;

				state = pattern[counter];
				timer = 60;
				playSound = true;
			}
		}
		break;

		case STATE_INVISIBLE:
		{
			visible = false;

			if (timer <= 0)
			{
				if (playSound)
				{
					aud_PlaySound(sounds[sndPi03]);
					playSound = false;
				}

				counter += 1;
				state = pattern[counter];
				timer = 15;

				//horizontal jump
				int gridx = x / 20;
				int lastgridx = gridx;

				do {
					gridx = (rand() % 16) + 1;
					x = gridx * 20;
				}
				while (gridx == lastgridx || solidCollision(x+10, y+10) || !solidCollision(x+10, y+30));
			}

			return;
		}
		break;
	}

	//collisions
	Rectangle mask = (Rectangle) { (int)x+4, (int)y+1, 12, 19 };

	if (state != STATE_FLASH)
		hero.hit(mask, 15);

	if (weaponList.collide(mask))
	{
		if (flag != 0)
			gameSetFlag(flag, true);
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Wizard::draw(float subFrame, float depth)
{
	if (visible)
	{
		gfx_DrawTile(images[imgEne01], x, y, 20, 20, 205 + (int)imageIndex);
	}
}