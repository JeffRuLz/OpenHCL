#include "labyrush.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../math.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

#define grav 0.0875

enum {
	STATE_WAIT,
	STATE_HOP
};

Labyrush::Labyrush(float x, float y, bool flip)
:Object(x,y)
{
	hp = 3;
	state = STATE_WAIT;
	timer = 0;
	counter = 0;
	direction = ((flip)? -1: 1);
	hsp = 0;
	vsp = 0;
	imageIndex = 0;
}

void Labyrush::update()
{
	Object::update();

	switch (state)
	{
		case STATE_WAIT:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.1, 2);

			//slide to a stop
			if (vsp >= 0)
			{
				if (hsp > 0)
				{
					if ((hsp -= 0.05) < 0)
						hsp = 0;
				}

				else if (hsp < 0)
				{
					if ((hsp += 0.05) > 0)
						hsp = 0;
				}
			}

			//change direction
			if (hsp != 0)
				direction = sign(hsp);

			//hero is close
			if (hsp == 0)
			{
				Rectangle area = (Rectangle){ (int)x-50, (int)y-15, 120, 50 };
				if (collision(hero.getX(), hero.getY(), area))
				{
					state = STATE_HOP;
					counter = 0;
					direction = ((hero.getX() > x+10)? 1: -1);
				}
			}
		}
		break;

		case STATE_HOP:
		{
			hsp = direction;

			//animate
			imageIndex = ((vsp >= 0)? 1: 2);

			//land on floor
			Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 16, 16 };
			mask.y += 1;
			Rectangle collide;
			if (solidCollision(mask, &collide))
			{
				y = collide.y - 20;
				counter += 1;
				vsp = ((counter != 3)? -0.75: -2);

				if (counter == 4)
				{
					state = STATE_WAIT;
					counter = 0;
					vsp = 0;
				}
				else
				{
					aud_PlaySound(sounds[sndPi05]);
					direction = ((hero.getX() > x+10)? 1: -1);
				}
			}
		}
		break;
	}

	//movement
	if (hsp)
	{
		x += hsp;

		Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 16, 16 };
		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			x = collide.x - 18 * sign(hsp);

			if (vsp <= 0)
				hsp *= -1;
		}
	}

	if (vsp)
	{
		y += vsp;

		Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 16, 16 };
		Rectangle collide;
		
		if (solidCollision(mask, &collide))
		{
			if (vsp > 0)
			{
				y = collide.y - 20;
				vsp = 0;
			}
			else
			{
				y = collide.y + 20;
			}
		}
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 16, 16 };

	mask.y += 1;
	if (!solidCollision(mask))
		vsp += grav;
	mask.y -= 1;

	if (hero.hit(mask, 10))
		hero.stun();

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

void Labyrush::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 22 + (int)imageIndex + (3 * (direction == -1));

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, tile);
}