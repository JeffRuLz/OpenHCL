#include "gyra.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"

enum {
	STATE_CIRCLE,
	STATE_ATTACK,
	STATE_OVAL,
	STATE_DEAD
};

static int tailIndex[] = { 118, 104, 89, 74, 59, 44, 29, 14, 0 };

Gyra::Gyra(float x, float y, int flag)
:Object(x,y)
{
	hp = 50;
	//hp = 3;
	this->flag = flag;
	state = STATE_CIRCLE;
	timer = -1;
	blinkTimer = 0;
	counter = 0;
	imageIndex = 0;
	ang = 195;
	targx = 0;
	targy = 0;
	
	for (int i = 0; i < 120; i++)
	{
		bodyx[i] = this->x;
		bodyy[i] = this->y;
	}
}

void Gyra::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

void Gyra::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	if (blinkTimer > 0)
		blinkTimer -= 1;

	switch (state)
	{
		case STATE_CIRCLE:
		{
			if (timer == -1)
				timer = 240;

			ang -= 1.5;
			if (ang < 0) ang += 360;

			x += lengthdir_x(1, ang);
			y += lengthdir_y(1, ang);

			if (timer <= 0)
			{
				state = STATE_ATTACK;
				timer = -1;
			}
		}
		break;

		case STATE_ATTACK:
		{
			//setup
			if (timer == -1)
			{
				timer = 120;

				targx = hero.getX();
				targy = hero.getY();

				if (distance(x,y,targx,targy) <= 60)
				{
					ang = angle(x,y,hero.getX(),hero.getY()) + 180;
					if (ang > 360) ang -= 360;
					if (ang < 0) ang += 360;
				}
			}

			//move towards target
			float targetAngle = ang - angle(x,y,targx,targy);
			if (targetAngle > 360) targetAngle -= 360;
			if (targetAngle < 0) targetAngle += 360;

			if (targetAngle < 180)
				ang -= 2;
			else
				ang += 2;

			float speed = 1.5;
			x += lengthdir_x(speed, ang);
			y += lengthdir_y(speed, ang);

			if (timer <= 0)
			{
				state = STATE_OVAL;
				timer = -1;
			}
		}
		break;

		case STATE_OVAL:
		{
			if (timer == -1)
				timer = 240;

			ang -= 1.5;
			if (ang > 360) ang -= 360;
			if (ang < 0) ang += 360;

			x += lengthdir_x(1.5, ang);
			y += lengthdir_y(1, ang);

			if (timer <= 0)
			{
				state = STATE_CIRCLE;
				timer = -1;
			}
		}
		break;

		case STATE_DEAD:
		{
			blinkTimer = timer;

			if (timer % 12 == 0)
			{
				aud_PlaySound(sounds[sndBom01]);
				effectList.add(new Poof(bodyx[tailIndex[counter]], bodyy[tailIndex[counter]]));
				counter++;

				if (counter > 8)
					dead = true;
			}

			return;
		}
		break;
	}

	//reposition body
	for (int i = 120-1; i > 0; i--)
	{
		bodyx[i] = bodyx[i-1];
		bodyy[i] = bodyy[i-1];
	}

	bodyx[0] = x;
	bodyy[0] = y;

	//collisions
	for (int i = 0; i < 9; i++)
	{
		Circle mask = (Circle){ (int)bodyx[tailIndex[i]], (int)bodyy[tailIndex[i]], 12 };

		if (hero.hit(mask, 20))
		{
			if (i == 0 || i == 8)
				hero.poison();
		}

		if (weaponList.collide(mask, this))
		{
			hitTimer = 15;

			if (i != 0)
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
					timer = 110;
					counter = 0;
					state = STATE_DEAD;
				}
			}

			break;
		}
	}
}

void Gyra::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;

	if (state == STATE_DEAD)
		subFrame = 1;

	for (int i = 0; i < 9; i++)
	{
		if (state == STATE_DEAD && counter > i)
			continue;

		float dx = lerp(bodyx[tailIndex[i]], bodyx[tailIndex[i]+1], subFrame);
		float dy = lerp(bodyy[tailIndex[i]], bodyy[tailIndex[i]+1], subFrame);

		int tile[] = { 4, 2, 2, 2, 2, 2, 2, 2, 0 };
		gfx_DrawTile(images[imgBoss], dx-20, dy-20, 40, 40, tile[i] + (int)imageIndex);
	}
}