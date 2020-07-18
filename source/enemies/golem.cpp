#include "golem.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../objects/collectable.hpp"
#include "../effects/brokenrock.hpp"

enum {
	STATE_ROLL,
	STATE_TRANSFORM
};

static bool playSound = true;

Golem::Golem(float x, float y, bool flip)
:Object(x,y)
{
	hp = 4;
	state = STATE_ROLL;
	direction = ((flip)? -1: 1);
	imageIndex = 0;

	playSound = true;
}

void Golem::update()
{
	Object::update();

	switch (state)
	{
		case STATE_ROLL:
		{
			//animate
			imageIndex += 0.2 * direction;
			if (imageIndex >= 8) imageIndex -= 8;
			if (imageIndex < 0) imageIndex += 8;

			//movement
			x += direction / 2.f;

			//check wall / ledge
			if (solidCollision(x+10+10*direction, y+10) || !solidCollision(x+10+10*direction, y+30))
			{
				if (playSound)
				{
					aud_PlaySound(sounds[sndPi10]);
					playSound = false;
				}
				state = STATE_TRANSFORM;
				imageIndex = 0;
			}
		}
		break;

		case STATE_TRANSFORM:
		{
			playSound = true;
			
			if ((imageIndex += 0.2) >= 12)
			{
				state = STATE_ROLL;
				imageIndex = 1.5;
				direction *= -1;
			}
		}
		break;
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+1, (int)y+3, 18, 17 };

	hero.hit(mask, 15);

	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
		{
			aud_PlaySound(sounds[sndHit02]);
		}
		else
		{
			aud_PlaySound(sounds[sndBom02]);
			rockSmashEffect(x+10, y+10);
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Golem::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	int tile = 72 + (int)imageIndex;

	if (state == STATE_TRANSFORM)
	{
		int animation[] = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 2, 1, 0 };
		tile = 118 + animation[(int)imageIndex];
	}

	gfx_DrawTile(images[imgEne01], dx, y + (state == STATE_ROLL), 20, 20, tile);
}