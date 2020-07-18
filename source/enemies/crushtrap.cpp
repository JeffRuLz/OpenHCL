#include "crushtrap.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/tink.hpp"
#include "../effects/brokenrock.hpp"
#include "../objects/collectable.hpp"

#define grav 0.15

enum {
	TYPE_AUTOMATIC,
	TYPE_VERTICAL,
	TYPE_HORIZONTAL
};

enum {
	STATE_WAIT,
	STATE_FALL,
	STATE_RISE,
	STATE_DONE
};

CrushTrap::CrushTrap(float x, float y, int type, bool flip, int waitTime, int delay)
:Object(x,y)
{
	hp = 3;
	this->type = type;
	imageIndex = 0;
	state = STATE_WAIT;
	timer = delay;
	this->waitTime = waitTime;
	direction = ((flip)? -1: 1);
	speed = 0;
	blinkTimer = 0;
	startx = this->x;
	starty = this->y;
}

void CrushTrap::update()
{
	Object::update();

	if ((blinkTimer -= 1) < 0)
		blinkTimer = 0;

	//animate
	imageIndex = fmod(imageIndex + 0.1, 4);

	switch (state)
	{
		case STATE_WAIT:
		{
			speed = 0;
			
			if (type == TYPE_VERTICAL)
			{
				Rectangle area = (Rectangle){ (int)x-20, (int)y, 60, 80 };
				if (collision(hero.getX(), hero.getY(), area))
					state = STATE_FALL;
			}
			else
			{
				if ((timer -= 1) <= 0)
					state = STATE_FALL;
			}
		}
		break;

		case STATE_FALL:
		{
			if ((speed += grav) > 4)
				speed = 4;

			if (type == TYPE_HORIZONTAL)
				x += speed * direction;
			else
				y += speed;

			//collide with solid
			Rectangle collide;
			Rectangle mask = (Rectangle){ (int)x+1, (int)y+1, 18, 18 };
			
			if (solidCollision(mask, &collide))
			{
				aud_PlaySound(sounds[sndHit07]);

				if (type == TYPE_HORIZONTAL)
				{
					if (direction == 1)
					{
						x = collide.x - 19;
						effectList.add(new Tink(x+20, y+10));
					}
					else
					{
						x = collide.x + 19;
						effectList.add(new Tink(x, y+10));
					}
				}
				else
				{
					y = collide.y - 19;
					effectList.add(new Tink(x+10, y+20));
				}

				//
				if (type == TYPE_VERTICAL)
					state = STATE_DONE;
				else
				{
					state = STATE_RISE;
					timer = waitTime;
				}
			}
		}
		break;

		case STATE_RISE:
		{
			if (timer > 0)
			{
				timer -= 1;
			}
			else
			{
				bool end = false;

				if (type == TYPE_AUTOMATIC)
				{
					y -= 1;
					
					if (y <= starty)
					{
						y = starty;
						end = true;
					}
				}
				else if (type == TYPE_HORIZONTAL)
				{
					x -= 1 * direction;

					if ((direction == 1 && x <= startx) ||
						(direction == -1 && x >= startx))
					{
						x = startx;
						end = true;
					}
				}

				if (end)
				{
					timer = waitTime;
					state = STATE_WAIT;
				}
			}
		}
		break;

		case STATE_DONE:
		{

		}
		break;
	}

	//collision
	Rectangle mask = (Rectangle){ (int)x+1, (int)y+1, 18, 18 };

	hero.hit(mask, 15);

	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;

		if (!gameHasItem(ITEM_BLUE_SCROLL))
		{
			aud_PlaySound(sounds[sndHit03]);
		}
		else
		{
			hp -= 1;
			blinkTimer = 15;

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
}

void CrushTrap::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int animation[] = { 0, 1, 2, 1 };

	gfx_DrawTile(images[imgChr20], dx, dy, 20, 20, 166 + animation[(int)imageIndex]);
}