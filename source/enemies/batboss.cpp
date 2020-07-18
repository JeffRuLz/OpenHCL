#include "batboss.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "statue.hpp"
#include "../effects/poof.hpp"

enum {
	STATE_HORIZONTAL,
	STATE_SHOOT,
	STATE_STOMP,
	STATE_CHASE,
	STATE_RISE,
	STATE_DEAD
};

BatBoss::BatBoss(float x, float y, int flag)
:Object(x,y)
{
	hp = 35;
	//hp = 5;

	state = STATE_HORIZONTAL;
	this->flag = flag;
	timer = 90;
	bobRot = 0;
	hsp = 1;
	vsp = 0;
	imageIndex = 0;
	ypos = this->y - 11;
	stompNext = true;
}

void BatBoss::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

void BatBoss::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	//animate
	if (state == STATE_STOMP || state == STATE_CHASE)
	{
		//twister
		imageIndex = max(2, imageIndex);
		if ((imageIndex += 0.2) >= 5)
			imageIndex -= 3;

	}
	else
	{
		//flap wings
		imageIndex = fmod(imageIndex + 0.1, 2);
	}

	switch (state)
	{
		case STATE_HORIZONTAL:
		{
			x += hsp;

			//hit wall
			int direction = ((hsp > 0)? 1: -1);

			if (solidCollision(x+direction*20, y+10))
				hsp *= -1;

			//bob vertically (large)
			bobRot = (bobRot + 2) % 360;
			y = ypos + lengthdir_y(20, bobRot);

			//slow to a halt
			if (timer <= 30)
			{
				float friction = 0.015;
				if (hsp > 0)
				{
					if ((hsp -= friction) <= 0)
						hsp = 0;
				}
				else if (hsp < 0)
				{
					if ((hsp += friction) >= 0)
						hsp = 0;
				}

				if (timer <= 0 && hsp == 0 && bobRot <= 2)
				{
					state = STATE_SHOOT;
					timer = 120;
					ypos = y;
				}
			}
		}
		break;

		case STATE_SHOOT:
		{
			//bob vertically (small)
			bobRot = (bobRot + 2) % 360;
			y = ypos + lengthdir_y(10, bobRot);

			//shoot flame
			if (timer == 60)
			{
				aud_PlaySound(sounds[sndShot03]);
				float ang = angle(x, y, hero.getX(), hero.getY());
				enemyList.add(new EnemyFireBall(x, y, ang, 1.5));
				enemyList.add(new EnemyFireBall(x, y, ang - 15, 1.5));
				enemyList.add(new EnemyFireBall(x, y, ang + 15, 1.5));
			}

			//next state
			if (timer <= 0 && bobRot <= 2)
			{
				if (stompNext)
				{
					state = STATE_STOMP;
					vsp = -2;
					aud_PlaySound(sounds[sndShot06]);
				}
				else
				{
					state = STATE_HORIZONTAL;
					timer = 60;
					hsp = ((hero.getX() < x)? -1: 1);
				}

				stompNext = !stompNext;
			}
		}
		break;

		case STATE_STOMP:
		{
			y += vsp;
			vsp = min(vsp + 0.05, 6);

			//hit floor
			Rectangle collide;
			if (solidCollision(x, y+24, &collide))
			{
				y = collide.y - 24;
				state = STATE_CHASE;
				timer = 120;
				aud_PlaySound(sounds[sndHit04]);
				gameSetQuake(30);
				hsp = 0.5 * ((hero.getX() > x)? 1: -1);
			}
		}
		break;

		case STATE_CHASE:
		{
			x += hsp;

			//hit wall
			int direction = ((hsp > 0)? 1: -1);
			if (timer <= 0 || solidCollision(x + direction * 20, y))
			{
				state = STATE_RISE;
				timer = 90 + (rand() % 60);
			}			
		}
		break;

		case STATE_RISE:
		{
			y -= 0.5;

			if (timer <= 0)
			{
				state = STATE_HORIZONTAL;
				ypos = y;
				bobRot = 0;
				timer = 90;
				hsp = ((hero.getX() > x)? 1: -1);
			}
		}
		break;

		case STATE_DEAD:
		{
			y += 0.1;

			if (timer % 12 == 0)
			{
				int ex = x - 20 + (rand() % 40);
				int ey = y - 20 + (rand() % 40);
				effectList.add(new Poof(ex,ey));
				aud_PlaySound(sounds[sndBom01]);
			}

			if (timer <= 0)
			{
				aud_StopMusic();
				gameSetFlag(flag, true);
				dead = true;
			}

			return;
		}
		break;
	}

	Rectangle mask = (Rectangle){ (int)x-20, (int)y-20, 40, 40 };
	hero.hit(mask, 30);

	mask = (Rectangle){ (int)x-30, (int)y-23, 60, 42 };
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

void BatBoss::draw(float subFrame, float depth)
{
	if (hitTimer % 2 || (state == STATE_DEAD && timer % 2))
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgBoss], dx-32, dy-24, 64, 48, (int)imageIndex);
}