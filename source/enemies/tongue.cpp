#include "tongue.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_WALK,
	STATE_WINDUP,
	STATE_LICK
};

Tongue::Tongue(float x, float y, bool flip)
:Object(x,y)
{
	hp = 2;
	state = STATE_WALK;
	timer = 0;
	direction = ((flip)? -1: 1);
	imageIndex = 0;
	tongueImageIndex = 0;
}

void Tongue::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	Rectangle tongueMask = (Rectangle){ -100, -100, 0, 0 };

	switch (state)
	{
		case STATE_WALK:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.1, 2);

			//movement
			x += 0.25 * direction;

			//hit wall / on ledge
			if (solidCollision(x+10+8*direction, y+10) || !solidCollision(x+10+10*direction, y+30))
				direction *= -1;

			//check if player is near
			if (timer <= 0)
			{
				Rectangle area = (Rectangle){ (int)x-20, (int)y, 60, 60 };
				if (collision(hero.getX(), hero.getY(), area))
				{
					state = STATE_WINDUP;
					imageIndex = 4;
					timer = 20;
				}
			}
		}
		break;

		case STATE_WINDUP:
		{
			imageIndex = 4;

			if (timer <= 0)
			{
				state = STATE_LICK;
				aud_PlaySound(sounds[sndGet01]);
				imageIndex = 5;
				timer = 40;
			}
		}
		break;

		case STATE_LICK:
		{
			if (timer >= 0 && timer <= 39)
			{
				//animate tongue
				int animation[40] = {
					0, 1, 1, 2, 2, 3, 3, 4, 4, 4,
					4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
					4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 
					2, 2, 1, 1, 0, 0, 5, 5, 5, 5
				};

				int tongueLen[] = { 
					12, 22, 32, 35, 36, 4
				};

				tongueImageIndex = animation[40 - 1 - timer];
				tongueMask = (Rectangle) {
					(int)x+6,
					(int)y+17,
					8,
					tongueLen[(int)tongueImageIndex]
				};
			}

			if (timer <= 0)
			{
				state = STATE_WALK;
				timer = 120;
				imageIndex = 0;
				tongueImageIndex = 0;
			}
		}
		break;
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+3, (int)y+6, 14, 14 };
	
	if (!hero.hit(mask, 10) && state == STATE_LICK)
		hero.hit(tongueMask, 10);

	bool hurt = false;
	if (weaponList.collide(mask, this))
		hurt = true;
	else if (state == STATE_LICK && weaponList.collide(tongueMask, this))
		hurt = true;

	if (hurt)
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
			aud_PlaySound(sounds[sndHit02]);
		else
		{
			aud_PlaySound(sounds[sndBom01]);
			effectList.add(new Poof(x+10,y));
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Tongue::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);

	int tile = 90 + (int)imageIndex;

	if (state == STATE_WALK && direction == -1)
		tile += 2;

	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, tile);

	//draw tongue
	if (state == STATE_LICK)
		gfx_DrawTile(images[imgChr2040], dx-1*depth, y+14, 20, 40, 5 + (int)tongueImageIndex);
}