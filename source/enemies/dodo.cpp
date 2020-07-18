#include "dodo.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/dust.hpp"
#include "../effects/poof.hpp"

#define accel 0.03
#define grav 0.1
#define maxspd 1.5

enum {
	STATE_IDLE,
	STATE_JUMP,
	STATE_CHASE,
	STATE_TURN,
	STATE_SLIDE,
	STATE_DEAD
};

Dodo::Dodo(float x, float y, int flag)
:Object(x,y)
{
	hp = 45;
	//hp = 5;

	state = STATE_JUMP;
	direction = (hero.getX() > this->x)? 1: -1;
	timer = 0;
	this->flag = flag;
	hsp = 0;
	vsp = -3;
	imageIndex = 0;
	onGround = false;
	chaseNext = true;
	highJumpNext = true;
}

void Dodo::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

static Rectangle _getMask(float x, float y)
{
	return (Rectangle){ (int)x-4, (int)y-8, 28, 28 };
}

void Dodo::update()
{
	Object::update();

	switch (state)
	{
		case STATE_IDLE:
		{
			imageIndex = fmod(imageIndex + 0.1, 4);

			if ((timer -= 1) <= 0)
			{
				if (chaseNext)
				{
					timer = 120 + (rand() % 20) * 5;
					state = STATE_CHASE;
				}
				else
				{
					timer = 30;
					state = STATE_JUMP;
					imageIndex = 0;
				}

				chaseNext = !chaseNext;
			}
		}
		break;

		case STATE_JUMP:
		{
			imageIndex = (vsp < 0);
			if (hsp != 0) direction = sign(hsp);

			if (timer > 0)
			{
				if ((timer -= 1) <= 0)
				{
					if (highJumpNext)
					{
						vsp = -3;
						hsp = 0.75 * direction;
					}
					else
					{
						vsp = -1.5;
						hsp = 1.25 * direction;
					}

					highJumpNext = !highJumpNext;
					aud_PlaySound(sounds[sndJump01]);
				}
			}
			else
			{
				if (onGround)
				{
					state = STATE_SLIDE;
					imageIndex = 0;
					aud_PlaySound(sounds[sndHit04]);
					gameSetQuake(30);
					effectList.add(new Dust(x,y+4,-1));
					effectList.add(new Dust(x,y+4,1));
				}
			}			
		}
		break;

		case STATE_CHASE:
		{
			imageIndex = fmod(imageIndex + 0.2, 4);

			hsp += accel * sign(hero.getX() - x);
			if (hsp > maxspd) hsp = maxspd;
			if (hsp < -maxspd) hsp = -maxspd;

			if (!onGround || ((timer -= 1) <= 0 && abs(hsp) >= 1))
			{
				state = STATE_SLIDE;
			}
			else if (direction != sign(hsp))
			{
				state = STATE_TURN;
				imageIndex = 0;
			}
		}
		break;

		case STATE_TURN:
		{
			if ((imageIndex += 0.3) >= 3)
			{
				direction *= -1;
				state = STATE_CHASE;
				imageIndex = 0;
			}
		}
		break;

		case STATE_SLIDE:
		{
			bool done = false;

			if (hsp > 0)
			{
				if ((hsp -= accel) <= 0)
					done = true;
			}
			else
			{
				if ((hsp += accel) >= 0)
					done = true;
			}

			if (done)
			{
				hsp = 0;
				state = STATE_IDLE;
				timer = 120;
			}
		}
		break;

		case STATE_DEAD:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.2, 4);

			y += 0.1;
			timer -= 1;

			//create effects
			if (timer % 12 == 0)
			{
				int ex = x - 12 + (rand() % 48);
				int ey = y - 20 + (rand() % 48);

				effectList.add(new Poof(ex,ey));
				aud_PlaySound(sounds[sndBom01]);
			}

			if (timer <= 0)
			{
				if (gameGetLevel() == 1)
					aud_StopMusic();

				gameSetFlag(flag, true);
				dead = true;
			}

			return;
		}
		break;
	}

	if (hsp)
	{
		x += hsp;

		Rectangle mask = _getMask(x,y);
		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			if (hsp > 0)
			{
				x = collide.x - (mask.w / 2) - 10;
			}
			else if (hsp < 0)
			{
				x = collide.x + 20 + (mask.w / 2) - 10;
			}

			if (state == STATE_JUMP || state == STATE_SLIDE)
			{
				hsp *= -1;
			}
			else if (state == STATE_CHASE)
			{
				state = STATE_SLIDE;
			}
		}
	}

	if (vsp)
	{
		y += vsp;

		Rectangle mask = _getMask(x,y);
		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			if (vsp > 0)
			{
				y = collide.y - 20;
				vsp = 0;
			}
			else if (vsp < 0)
			{
				y = collide.y + 20 + 8;
				vsp = 0;
			}
		}
	}

	{
		Rectangle mask = _getMask(x,y);
		mask.y += 1;
		onGround = solidCollision(mask);
	}

	if (onGround == false)
		vsp += grav;

	{
		Circle mask = (Circle){ (int)x+10, (int)y+10, 16 };
		hero.hit(mask, 30);
	}

	{
		Rectangle mask = (Rectangle){ (int)x-13, (int)y-12, 46, 32 };

		if (weaponList.collide(mask, this))
		{
			hp -= 1;
			hitTimer = 15;

			if (hp > 0)
				aud_PlaySound(sounds[sndHit02]);
			else
			{
				hitTimer = 0;
				state = STATE_DEAD;
				timer = 180;
			}
		}
	}
}

void Dodo::draw(float subFrame, float depth)
{
	if (hitTimer % 2 || (state == STATE_DEAD && timer % 2))
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 0;

	if (state == STATE_JUMP)
	{
		tile = 12 + ((direction == -1)? 2: 0) + (int)imageIndex;
	}

	else if (state == STATE_IDLE)
	{
		if (direction == 1)
		{
			int animation[] = { 0, 6, 7, 6 };
			tile = animation[(int)imageIndex];
		}
		else
		{
			int animation[] = { 3, 8, 9, 8 };
			tile = animation[(int)imageIndex];
		}
	}

	else if (state == STATE_CHASE)
	{
		int animation[] = { 0, 1, 0, 2 };
		tile = animation[(int)imageIndex] + ((direction == -1)? 3: 0);
	}

	else if (state == STATE_TURN)
	{
		if (direction == 1)
		{
			int animation[] = { 18, 19, 20 };
			tile = animation[(int)imageIndex];
		}
		else
		{
			int animation[] = { 20, 19, 18 };
			tile = animation[(int)imageIndex];
		}
	}

	else if (state == STATE_SLIDE)
	{
		tile = 12 + ((direction == -1)? 2: 0);
	}

	else if (state == STATE_DEAD)
	{
		int animation[] = { 0, 1, 0, 2 };
		tile = animation[(int)imageIndex] + ((direction == -1)? 3: 0);
	}

	gfx_DrawTile(images[imgBoss], dx-14, dy-21, 48, 48, tile);

	//Rectangle mask = _getMask(x,y);
	//gfx_DrawRect(mask.x, mask.y, mask.x+mask.w, mask.y+mask.h, (Color){0xff, 0, 0});
}