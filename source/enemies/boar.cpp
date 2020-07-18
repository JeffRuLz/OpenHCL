#include "boar.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/poof.hpp"
#include "../effects/dust.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_WAIT,
	STATE_CHARGE,
	STATE_RUN
};

Boar::Boar(float x, float y)
:Object(x,y)
{
	hp = 3;
	state = STATE_WAIT;
	timer = 0;
	direction = 1;
	imageIndex = 0;
	hsp = 0;
}

void Boar::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	switch (state)
	{
		case STATE_WAIT:
		{
			//dance
			imageIndex = fmod(imageIndex + 0.15, 8);

			//hero is near
			Rectangle area = (Rectangle){ (int)x-40, (int)y-20, 100, 40 };
			if (collision(hero.getX(), hero.getY(), area))
			{
				state = STATE_CHARGE;
				imageIndex = 0;
				timer = 60;
				direction = ((hero.getX() > x+10)? 1: -1);
			}
		}
		break;

		case STATE_CHARGE:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.2, 2);

			//sound
			if (timer % 10 == 0)
				aud_PlaySound(sounds[sndShot01]);

			if (timer % 16 == 0)
				effectList.add(new Dust(x-10*direction, y+4, -direction));

			if (timer <= 0)
			{
				state = STATE_RUN;
				hsp = 1.5;
			}
		}
		break;

		case STATE_RUN:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.2, 2);

			//movement
			x += hsp * direction;

			//turn on wall / ledge
			if (solidCollision(x+10+10*direction, y+10) || !solidCollision(x+10+10*direction, y+30))
				direction *= -1;

			//slow down
			if (hsp > 0)
				hsp -= 0.025;
			else
				state = STATE_WAIT;
		}
		break;
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+6, 16, 14 };

	hero.hit(mask, 30);

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
			aud_PlaySound(sounds[sndBom01]);
			effectList.add(new Poof(x+10,y));
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Boar::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	int tile = 144;

	if (state == STATE_WAIT)
	{
		int animation[] = { 0, 1, 2, 1, 0, 3, 4, 3 };
		tile += 4 + animation[(int)imageIndex];
	}
	else
	{
		tile += (int)imageIndex + 2 * (direction == -1);
	}

	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, tile);
}