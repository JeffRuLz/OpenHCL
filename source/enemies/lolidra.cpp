#include "lolidra.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../effects/sparkle.hpp"
#include "../effects/poof.hpp"

static int minionCount = 0;

enum {
	STATE_MINIONS,
	STATE_DISAPPEAR,
	STATE_APPEAR,
	STATE_DEAD
};

Lolidra::Lolidra(float x, float y, int flag)
:Object(x,y)
{
	hp = 75;
	//hp = 2;
	this->flag = flag;
	state = STATE_MINIONS;
	nextState = STATE_APPEAR;
	timer = 600;
	flashTimer = 0;
	counter = 0;
	visible = true;
	imageIndex = 0;
	hoverRot = 0;
	ypos = this->y;

	minionCount = 0;
}

void Lolidra::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

void Lolidra::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	if (flashTimer > 0 && state != STATE_DEAD)
	{
		flashTimer -= 1;
		visible = !visible;
	}

	//animate
	imageIndex = fmod(imageIndex + 0.1, 3);

	switch (state)
	{
		case STATE_MINIONS:
		{
			if (flashTimer <= 0)
			{
				visible = true;

				if (counter < 5)
				{
					counter += 1;
				}
				else if (minionCount < 10)
				{
					counter = 0;
					aud_PlaySound(sounds[sndPi02]);
					enemyList.add(new Minion(x,y));
				}
			}

			if (timer <= 0)
			{
				//blink before leaving
				if (flashTimer == 0)
				{
					flashTimer = 21;
					aud_PlaySound(sounds[sndPi10]);
				}

				if (flashTimer == 1)
				{
					state = STATE_DISAPPEAR;
					nextState = STATE_APPEAR;
					timer = 300;
					visible = false;
					flashTimer = -1;
				}
			}
		}
		break;

		case STATE_DISAPPEAR:
		{
			visible = false;

			if (timer <= 0)
			{
				aud_PlaySound(sounds[sndPi03]);
				setX(hero.getX());
				ypos = hero.getY() - 30;
				setY(ypos + lengthdir_y(2.5, hoverRot));
				state = nextState;
				flashTimer = 20;
				timer = ((state == STATE_APPEAR)? 180: 600);
			}

			return;
		}
		break;

		case STATE_APPEAR:
		{
			if (flashTimer <= 0)
				visible = true;

			if (timer <= 0)
			{
				if (flashTimer == 0)
				{
					flashTimer = 21;
					aud_PlaySound(sounds[sndPi10]);
				}

				if (flashTimer == 1)
				{
					state = STATE_DISAPPEAR;
					nextState = STATE_MINIONS;
					timer = 300;
					visible = false;
					flashTimer = -1;
				}
			}
		}
		break;

		case STATE_DEAD:
		{
			y += 0.1;

			if (timer % 12 == 0)
			{
				int ex = x - 30 + (rand() % 60);
				int ey = y - 30 + (rand() % 60);
				aud_PlaySound(sounds[sndBom01]);
				effectList.add(new Poof(ex,ey));
			}

			if (timer <= 0)
			{
				aud_StopMusic();
				if (flag != 0)
					gameSetFlag(flag, true);
				dead = true;
			}

			return;
		}
		break;
	}

	//hover
	hoverRot = (hoverRot + 5) % 360;
	y = ypos + lengthdir_y(2.5, hoverRot);

	//collisions
	Circle mask = (Circle){ (int)x, (int)y, 23 };

	hero.hit(mask, 30);

	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;
		hp -= 1;

		if (hp > 0)
			aud_PlaySound(sounds[sndHit02]);
		else
		{
			hitTimer = 0;
			timer = 180;
			state = STATE_DEAD;
		}
	}
}

void Lolidra::draw(float subFrame, float depth)
{
	if (hitTimer % 2 || (state != STATE_DEAD && !visible) || (state == STATE_DEAD && timer % 2 == 0))
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgBoss], dx-32, dy-32, 64, 64, (int)imageIndex);
}


enum {
	STATE_SPAWN,
	STATE_HOVER,
	STATE_TARGET
};

Minion::Minion(float x, float y)
:Object(x,y)
{
	state = STATE_SPAWN;
	timer = 0;
	direction = rand() % 360;
	speed = 4;
	imageIndex = 0;
	ypos = this->y;

	minionCount += 1;
}

Minion::~Minion()
{
	minionCount -= 1;
}

void Minion::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.2, 2);

	if (timer > 0)
		timer -= 1;

	switch (state)
	{
		case STATE_SPAWN:
		{
			x += lengthdir_x(speed, direction);
			y += lengthdir_y(speed, direction);

			if ((speed -= 0.15) <= 0)
			{
				ypos = y;
				speed = 0;
				direction = 0;
				state = STATE_HOVER;
				timer = 120;
			}
		}
		break;

		case STATE_HOVER:
		{
			direction = (direction + 5) % 360;
			y = ypos + lengthdir_y(5, direction);

			if (timer <= 0)
			{
				timer = 120;
				state = STATE_TARGET;
				speed = ((rand() % 2) + 1) / 2.f;
				direction = angle(x,y, hero.getX(),hero.getY());
			}
		}
		break;

		case STATE_TARGET:
		{
			x += lengthdir_x(speed, direction);
			y += lengthdir_y(speed, direction);

			if (timer <= 0)
			{
				effectList.add(new Sparkle(x,y));
				dead = true;
			}
		}
		break;
	}

	//collisions
	Circle mask = (Circle) { (int)x, (int)y, 5 };

	if (hero.hit(mask, 10))
		hero.poison();

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x,y));
		dead = true;
	}
}

void Minion::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	
	dx += (-2 + lengthdir_x(2, direction)) * depth;

	gfx_DrawTile(images[imgBoss], dx-16, dy-16, 32, 32, 20 + (int)imageIndex);
}