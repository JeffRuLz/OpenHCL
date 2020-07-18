#include "devil.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/poof.hpp"

enum {
	STATE_MOVE,
	STATE_SHOOT,
	STATE_DEAD
};

Devil::Devil(float x, float y, int flag)
:Object(x,y)
{
	hp = 100;
	//hp = 5;
	this->flag = flag;
	state = STATE_MOVE;
	timer = 60;
	blinkTimer = 0;
	counter = 0;
	direction = ((this->x < 160)? 1: -1);
	imageIndex = 0;
	hsp = 1.25;
	vsp = 0;

	bobSpeed = 1;
	bobRotSpeed = 3;
	bobRot = 90;

	tailAngle = 0;
	tailSpeed = 1;
	tailRot[0] = 0;
	tailRot[1] = 0;
}

void Devil::create()
{
	if (flag != 0 && gameGetFlag(flag))
		dead = true;
}

void Devil::update()
{
	Object::update();

	tailRot[0] = fmod(tailRot[0], 360);
	tailRot[1] = tailRot[0];

	if (blinkTimer > 0)
		blinkTimer -= 1;

	//animate
	imageIndex = fmod(imageIndex + 0.1, 2);

	//swing tail
	tailRot[0] += tailSpeed;
	tailAngle = 90 + 60 * lengthdir_x(1, tailRot[0]);

	switch (state)
	{
		case STATE_MOVE:
		{
			bobSpeed = 1;
			bobRotSpeed = 3;
			tailSpeed = 1;

			if (hsp != 0)
			{
				x += hsp * direction;

				if (hsp > 0)
				{
					if ((hsp -= 0.0085) <= 0)
						hsp = 0;
				}
				else if (hsp < 0)
				{
					if ((hsp += 0.0085) >= 0)
						hsp = 0;
				}

				if (hsp == 0)
				{
					timer = 60;
					counter += 1;
				}
			}
			//mid room pause
			else
			{
				if ((timer -= 1) <= 0)
				{
					if (counter < 2)
					{
						hsp = 1.25;
						direction = ((hero.getX() > x)? 1: -1);
					}
					else
					{
						direction *= -1;
						counter = 0;
						state = STATE_SHOOT;
						timer = 360;
					}
				}
			}
		}
		break;

		case STATE_SHOOT:
		{
			bobSpeed = 0.5;
			bobRotSpeed = 6;
			tailSpeed = 3;

			if (timer > 0)
				timer -= 1;

			if (timer % 120 == 0)
			{
				aud_PlaySound(sounds[sndShot03]);
				float ang = angle(x, y, hero.getX(), hero.getY());
				enemyList.add(new Orb(x,y, ang+22));
				enemyList.add(new Orb(x,y, ang+11));
				enemyList.add(new Orb(x,y, ang));
				enemyList.add(new Orb(x,y, ang-11));
				enemyList.add(new Orb(x,y, ang-22));
			}

			if (timer <= 0)
			{
				timer = -1;
				state = STATE_MOVE;
				hsp = 1.25;
				vsp = 1.5 * ((hero.getY() > y)? 1: -1);
			}
		}
		break;

		case STATE_DEAD:
		{
			tailSpeed = 6;

			y += 0.1;

			blinkTimer = timer;

			if (timer % 12 == 0)
			{
				int ex = x - 30 + (rand() % 60);
				int ey = y - 30 + (rand() % 60);
				effectList.add(new Poof(ex,ey));
				aud_PlaySound(sounds[sndBom01]);
			}

			if ((timer -= 1) <= 0)
			{
				if (flag != 0)
					gameSetFlag(flag, true);
				dead = true;
			}

			return;
		}
		break;
	}

	//bob up and down
	y += lengthdir_y(bobSpeed, bobRot);
	bobRot = fmod(bobRot + bobRotSpeed, 360);

	y += vsp;

	if (vsp > 0)
	{
		if ((vsp -= 0.05) <= 0)
			vsp = 0;
	}
	else if (vsp < 0)
	{
		if ((vsp += 0.05) <= 0)
			vsp = 0;
	}

	//collisions
	Circle headMask = (Circle){
		(int)x,
		(int)y,
		25
	};

	Circle tailMask = (Circle){
		(int)(x+lengthdir_x(70, tailAngle)),
		(int)(y+12+lengthdir_y(70, tailAngle)),
		14
	};

	if (!hero.hit(headMask, 50))
		if (hero.hit(tailMask, 25))
			hero.stun();

	if (weaponList.collide(headMask, this))
	{
		hitTimer = 15;
		blinkTimer = 15;
		hp -= 1;

		if (hp > 0)
		{
			aud_PlaySound(sounds[sndHit02]);
		}
		else
		{
			timer = 180;
			state = STATE_DEAD;
		}
	}
	
	if (weaponList.collide(tailMask, this))
	{
		hitTimer = 15;
		aud_PlaySound(sounds[sndHit03]);
	}
}

void Devil::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	float rot = lerp(tailRot[0], tailRot[1], subFrame);

	int tailLag[] = { 20, 16, 12, 5, 0 };
	int tile[] = { 20, 21 };

	for (int i = 0; i < 5; i++)
	{
		bool isTail = (i == 4);
		int tailLength = 14 * (i+1);
		float ang = 90 + (60 * lengthdir_x(1, rot - tailLag[i]));
		float tx = dx + lengthdir_x(tailLength, ang);
		float ty = dy + 12 + lengthdir_y(tailLength, ang);

		if (isTail)
			tx += -2 * depth;

		gfx_DrawTile(images[imgBoss], tx-16, ty-16, 32, 32, tile[isTail]);
	}

	//head
	dx += -2 * depth;
	gfx_DrawTile(images[imgBoss], dx-32, dy-32, 64, 64, (int)imageIndex);
}


Orb::Orb(float x, float y, float ang)
:Object(x,y)
{
	int spd = 2;
	hsp = lengthdir_x(spd, ang);
	vsp = lengthdir_y(spd, ang);
	imageIndex = 0;
}

void Orb::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 4);

	//movement
	x += hsp;
	y += vsp;

	//collisions
	Circle mask = (Circle){ (int)x, (int)y, 3 };

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x,y));
		dead = true;
	}
	else if (hero.hit(mask, 25))
	{
		hero.petrify();
	}

	//outside of screen
	if (x < -10 || y < -10 || x > 330 || y > 250)
		dead = true;
}

void Orb::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	dx += -2 * depth;

	int animation[] = { 0, 2, 0, 1 };

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 203 + animation[(int)imageIndex]);
}