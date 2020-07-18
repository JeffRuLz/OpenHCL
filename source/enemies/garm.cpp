#include "garm.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../effects/dust.hpp"
#include "../effects/poof.hpp"
#include "../effects/brokenrock.hpp"

enum {
	STATE_WAIT,
	STATE_BOUNCE,
	STATE_LEAP,
	STATE_WALL,
	STATE_STOMP,
	STATE_DEAD
};

Garm::Garm(float x, float y, int flag)
:Object(x+10,y+8)
{
	hp = 105;
	//hp = 5;
	state = 0;
	timer = 60;
	counter = 0;
	jumpCounter = 1;
	imageIndex = 0;
	hsp = 0;
	vsp = 0;
}

void Garm::update()
{
	Object::update();

	float grav = 0.125;

	switch (state)
	{
		case STATE_WAIT:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.05, 2);

			vsp = 0;
			hsp = 0;

			if ((timer -= 1) <= 0)
			{
				state = STATE_BOUNCE;
				counter = 0;
				vsp = -2.5;
				aud_PlaySound(sounds[sndPi09]);
			}
		}
		break;

		case STATE_BOUNCE:
		{
			Rectangle collide;
			if (solidCollision(x,y+32,&collide))
			{
				imageIndex = fmod(imageIndex + 0.05, 2);

				y = collide.y-32;
				vsp = 0;

				if ((timer -= 1) <= 0)
				{
					aud_PlaySound(sounds[sndPi09]);
					vsp = -2.5;
					counter += 1;
					timer = 3;
				}
			}
			else
			{
				imageIndex += 0.3;
				if (imageIndex < 7) imageIndex = 7;
				if (imageIndex >= 10) imageIndex -= 3;

				if (counter >= 2)
				{
					state = STATE_LEAP;
					timer = 60;
					counter = 0;
					vsp = -3;
					hsp = -4 + (8 * (hero.getX() > x));
				}
			}
		}
		break;

		case STATE_LEAP:
		{
			//animate
			imageIndex = 5 + (hsp < 0);

			if (counter < jumpCounter)
			{
				//hit wall
				Rectangle collide;
				if (solidCollision(x+22*sign(hsp),y,&collide))
				{
					x = (collide.x + 10) - (32 * sign(hsp));
					state = STATE_WALL;
					counter += 1;
					timer = 6;
				}
			}
			else
			{
				if (vsp >= 0)
				{
					state = STATE_STOMP;
					aud_PlaySound(sounds[sndWolf01]);
					vsp = -2;
				}
			}
		}
		break;

		case STATE_WALL:
		{
			//animate
			imageIndex = 3 + (x < 160);

			hsp = 0;
			vsp = 0;

			if ((timer -= 1) <= 0)
			{
				if (counter < jumpCounter)
				{
					hsp = 4;
					vsp = -3;
				}
				else
				{
					vsp = -3.5;
					hsp = 1;

					if (abs(x - hero.getX()) > 32)
						hsp += 1 + (rand() % 3);
				}

				if (x > 160)
					hsp *= -1;

				state = STATE_LEAP;
				aud_PlaySound(sounds[sndPi09]);
			}
		}
		break;

		case STATE_STOMP:
		{
			hsp = 0;

			//animate
			imageIndex += 0.3;
			if (imageIndex < 7) imageIndex = 7;
			if (imageIndex >= 10) imageIndex -= 3;

			//collide with floor
			Rectangle collide;
			if (solidCollision(x,y+32,&collide))
			{
				y = collide.y-32;
				state = STATE_WAIT;
				timer = 80;
				vsp = 0;

				aud_PlaySound(sounds[sndHit04]);
				gameSetQuake(30);

				enemyList.add(new GarmRock(x-32, y+22,   -1,   -2));
				enemyList.add(new GarmRock(x-16, y+22, -0.5, -2.5));				
				enemyList.add(new GarmRock(x+16, y+22,  0.5, -2.5));
				enemyList.add(new GarmRock(x+32, y+22,    1,   -2));

				effectList.add(new Dust(x-30, y+16, -1));
				effectList.add(new Dust(x+10, y+16,  1));

				jumpCounter = ((jumpCounter == 1)? 2: 1);
			}
		}
		break;

		case STATE_DEAD:
		{
			timer -= 1;
			hitTimer = timer;

			y += 0.1;

			//animate
			imageIndex += 0.3;
			if (imageIndex < 7) imageIndex = 7;
			if (imageIndex >= 10) imageIndex -= 3;			

			//create effects
			if (timer % 12 == 0)
			{
				int ex = x - 32 + (rand() % 64);
				int ey = y - 32 + (rand() % 64);
				effectList.add(new Poof(ex,ey));
				aud_PlaySound(sounds[sndBom01]);
			}

			if (timer <= 0)
				dead = true;

			return;
		}
		break;
	}

	//movement
	x += hsp;
	y += vsp;
	vsp += grav;

	//collisions
	Rectangle mask = (Rectangle){ (int)x-21, (int)y-21, 42, 48 };
	hero.hit(mask, 40);

	mask = (Rectangle){ (int)x-25, (int)y-28, 50, 60 };
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
			hitTimer = 0;
			state = STATE_DEAD;
			timer = 180;
		}
	}
}

void Garm::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgBoss], dx-32, dy-32, 64, 64, (int)imageIndex);
}


GarmRock::GarmRock(float x, float y, float hsp, float vsp)
:Object(x,y)
{
	hp = 3;
	counter = 0;
	imageIndex = 0;
	this->hsp = hsp;
	this->vsp = vsp;
}

void GarmRock::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.2, 4);

	//movement
	x += hsp;
	y += vsp;
	vsp += 0.05;

	//bounce
	if (counter == 0 && vsp >= 0)
	{
		Rectangle collide;
		if (solidCollision(x,y+10,&collide) && !solidCollision(x,y))
		{
			y = collide.y - 10;
			vsp = -1;
			aud_PlaySound(sounds[sndHit06]);
			counter += 1;
		}
	}

	Rectangle mask = (Rectangle){ (int)x-10, (int)y-10, 20, 20 };

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
			aud_PlaySound(sounds[sndBom02]);
			rockSmashEffect(x,y);
			dead = true;
		}
	}

	//out of room
	if (y >= 250)
		dead = true;
}

void GarmRock::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgChr32], dx-16, dy-16, 32, 32, 34 + (int)imageIndex);
}