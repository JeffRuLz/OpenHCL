#include "golgia.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"
#include "../effects/poof.hpp"

#define grav 0.075

enum {
	STATE_WAIT,
	STATE_SHOOT,
	STATE_JUMP,
	STATE_ROLL,
	STATE_DEAD
};

Golgia::Golgia(float x, float y, int flag)
:Object(x,y)
{
	hp = 35;
	//hp = 5;

	this->flag = flag;
	state = STATE_WAIT;
	rollState = 0;
	timer = 60;
	counter = -1;
	imageIndex = 0;
	hsp = 0;
	vsp = 0;
}

void Golgia::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

void Golgia::update()
{
	Object::update();

	int pattern[] = {
		STATE_SHOOT,
		STATE_JUMP,
		STATE_SHOOT,
		STATE_JUMP,
		STATE_ROLL,
		STATE_JUMP,
		STATE_SHOOT,
		STATE_JUMP,
		STATE_ROLL
	};

	if (timer > 0)
		timer -= 1;

	switch (state)
	{
		case STATE_WAIT:
		{
			imageIndex = 0;

			if (timer <= 0)
			{
				if ((counter += 1) >= 9)
					counter = 0;

				state = pattern[counter];
				timer = 0;
			}
		}
		break;

		case STATE_SHOOT:
		{
			aud_PlaySound(sounds[sndPi05]);

			float ang = angle(x, y, hero.getX(), hero.getY());
			enemyList.add(new ElectricOrb(x,y,ang-45));
			enemyList.add(new ElectricOrb(x,y,ang-22.5));
			enemyList.add(new ElectricOrb(x,y,ang));
			enemyList.add(new ElectricOrb(x,y,ang+22.5));
			enemyList.add(new ElectricOrb(x,y,ang+45));

			state = STATE_WAIT;
			timer = 20;
		}
		break;

		case STATE_JUMP:
		{
			imageIndex = 1;

			//jumping up or jumping down
			bool jumpUp = (counter < 4);

			//jump on first frame
			if (timer == 0)
			{
				timer = -1;

				aud_PlaySound(sounds[sndJump02]);
				float jumpSpeed[] = { -0.75, -3.25 };
				vsp = jumpSpeed[jumpUp];
			}

			//vertical movement
			y += (vsp += grav);
			vsp = min(vsp, 3);

			//land on ground
			if ((jumpUp && vsp > 0) || (!jumpUp && vsp >= 3))
			{
				Rectangle collide;
				if (solidCollision(x, y+20, &collide))
				{
					y = collide.y - 20;
					state = STATE_WAIT;
					timer = 30;
					//shorten timer if the next state is roll
					if (pattern[counter+1] == STATE_ROLL)
						timer = 3;
				}
			}
		}
		break;

		case STATE_ROLL:
		{
			//animate
			imageIndex += 0.25 * sign(hsp);
			if (imageIndex < 2) imageIndex += 4;
			if (imageIndex >= 6) imageIndex -= 4;

			//small hop
			if (rollState == 0)
			{
				if (timer == 0)
				{
					timer = -1;
					aud_PlaySound(sounds[sndHit04]);
					vsp = -1.25;
					hsp = 4 * ((x < 160)? 1: -1);
				}

				y += (vsp += grav);

				//check if on ground
				if (vsp > 0)
				{
					Rectangle collide;
					if (solidCollision(x, y+20, &collide))
						rollState = 1;
				}
			}

			//roll
			else if (rollState == 1)
			{
				x += hsp;

				Rectangle collide;
				if (solidCollision(x+20*sign(hsp), y, &collide))
				{
					aud_PlaySound(sounds[sndHit04]);
					x = collide.x + 10 + (20 * -sign(hsp));
					rollState = 2;
					timer = 0;
				}
			}

			//bounce off wall
			else if (rollState == 2)
			{
				imageIndex = 1;

				if (timer == 0)
				{
					timer = -1;
					vsp = -1;
					hsp = 1.25 * -sign(hsp);
				}

				//movement
				x += hsp;
				y += (vsp += grav);

				//touch ground
				if (vsp > 0)
				{
					Rectangle collide;
					if (solidCollision(x, y+20, &collide))
					{
						y = collide.y - 20;
						state = STATE_WAIT;
						rollState = 0;
						timer = 60;
					}
				}
			}
		}
		break;

		case STATE_DEAD:
		{
			imageIndex = 1;

			y += 0.1;

			if (timer % 12 == 0)
			{
				int ex = x - 20 + (rand() % 40);
				int ey = y - 20 + (rand() % 40);
				aud_PlaySound(sounds[sndBom01]);
				effectList.add(new Poof(ex,ey));
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

	Circle mask = (Circle){ (int)x, (int)y, 16 };

	hero.hit(mask, 30);

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

void Golgia::draw(float subFrame, float depth)
{
	if ((state != STATE_DEAD && hitTimer % 2) || (state == STATE_DEAD && timer %2))
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgBoss], dx-20, dy-20, 40, 40, (int)imageIndex);
}


ElectricOrb::ElectricOrb(float x, float y, float ang)
:Object(x,y)
{
	float speed = 1.5;
	hsp = lengthdir_x(speed, ang);
	vsp = lengthdir_y(speed, ang);
	imageIndex = 0;
}

void ElectricOrb::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.25, 3);

	//
	x += hsp;
	y += vsp;

	//collision
	Circle mask = (Circle){ (int)x, (int)y, 8 };

	//shield collision
	if (collision(hero.getShieldMask(), mask))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//hero collision
	else
	{
		if (hero.hit(mask, 25))
			hero.stun();
	}

	//outside of screen
	if (x < -10 || x > 330 || y < -1 || y > 250)
		dead = true;
}

void ElectricOrb::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 1 + (int)imageIndex);
}