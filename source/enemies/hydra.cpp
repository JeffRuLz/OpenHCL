#include "hydra.hpp"
#include "input.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../math.hpp"
#include "../text.hpp"
#include "../effects/poof.hpp"
#include "../effects/dust.hpp"
#include "../effects/tink.hpp"

enum {
	STATE_INTRO,
	STATE_WAIT,
	STATE_FLAIL,
	STATE_HOP,
	STATE_GOOP,
	STATE_STOMP,
	STATE_ELECTRIC,
	STATE_HEADLESS,
	STATE_DEATH
};

enum {
	STATE_HEAD_NORMAL,
	STATE_HEAD_FLAIL,
	STATE_HEAD_GOOP,
	STATE_HEAD_ELECTRIC,
	STATE_HEAD_DEATH
};

enum {
	BOTTOM_RIGHT = 0,
	BOTTOM_LEFT  = 1,
	TOP_RIGHT 	 = 2,
	TOP_LEFT 	 = 3
};

static float goopImage = 0;
static float boltImage = 0;

Hydra::Hydra(float x, float y, int flag)
:Object(160,-64)
{
	hp = 10;
	//hp = 5;
	this->flag = flag;
	state = STATE_INTRO;
	timer = 50;
	counter = 0;
	blinkTimer = 0;
	onground = false;
	headless = false;
	hsp = 0;
	vsp = 0;
	imageIndex = 0;

	head[BOTTOM_RIGHT] = HydraHead(this, BOTTOM_RIGHT);
	head[BOTTOM_LEFT]  = HydraHead(this, BOTTOM_LEFT);
	head[TOP_RIGHT]    = HydraHead(this, TOP_RIGHT);
	head[TOP_LEFT]     = HydraHead(this, TOP_LEFT);

	loadBossGraphics(9);
}

void Hydra::update()
{
	Object::update();

	float grav = 0.1;
	float fric = 0.05;

	goopImage = fmod(goopImage + 0.15, 3);
	boltImage = fmod(boltImage + 0.5, 4);

	if (timer > 0)
		timer -= 1;

	if (blinkTimer > 0)
		blinkTimer -= 1;

	switch (state)
	{
		case STATE_INTRO:
		{
			if (timer <= 0)
			{
				//land on ground
				if (onground)
				{
					state = STATE_WAIT;
					timer = 120;
				}
			}
		}
		break;

		case STATE_WAIT:
		{
			if (headless == false &&
				head[BOTTOM_RIGHT].isDead() &&
				head[BOTTOM_LEFT].isDead() &&
				head[TOP_RIGHT].isDead() &&
				head[TOP_LEFT].isDead())
			{
				headless = true;
				state = STATE_HEADLESS;
				counter = 0;
				timer = 75;
			}

			if (timer <= 0)
			{
				int pattern[] = {
					STATE_FLAIL,
					STATE_HOP,
					STATE_FLAIL,
					STATE_GOOP,
					STATE_FLAIL,
					STATE_STOMP,
					STATE_FLAIL,
					STATE_ELECTRIC,
					STATE_STOMP,
					-1
				};

				if (pattern[counter] == -1)
					counter = 0;

				switch (pattern[counter])
				{
					case STATE_FLAIL:
					{
						timer = 120;
						head[BOTTOM_RIGHT].setState(STATE_HEAD_FLAIL, 120);
						head[BOTTOM_LEFT].setState(STATE_HEAD_FLAIL, 120);
						head[TOP_RIGHT].setState(STATE_HEAD_FLAIL, 120);
						head[TOP_LEFT].setState(STATE_HEAD_FLAIL, 120);
					}
					break;

					case STATE_HOP:
					{
						timer = 190;
						vsp = -1;
						hsp = 1 * ((hero.getX() > x)? 1: -1);
						aud_PlaySound(sounds[sndPi06]);
					}
					break;

					case STATE_GOOP:
					{
						timer = 230;
						head[BOTTOM_LEFT].setState(STATE_HEAD_GOOP, 15*8+1);
						head[BOTTOM_RIGHT].setState(STATE_HEAD_GOOP, 15*8+1);
					}
					break;

					case STATE_STOMP:
					{
						state = STATE_STOMP;
						vsp = -4;
						onground = false;
						timer = 20*11;
					}
					break;

					case STATE_ELECTRIC:
					{
						timer = 180;
						head[TOP_LEFT].setState(STATE_HEAD_ELECTRIC, 40*4+1);
						head[TOP_RIGHT].setState(STATE_HEAD_ELECTRIC, 40*4+1);
					}
					break;
				}

				counter += 1;
			}
		}
		break;

		case STATE_STOMP:
		{
			if (!onground)
			{
				hsp = 0;
				timer += 1;
			}
			else
			{
				if (timer % 20 == 0)
					enemyList.add(new HydraRock());
			}

			if (timer <= 0)
			{
				state = ((headless)? STATE_HEADLESS: STATE_WAIT);
				timer = 110;
			}
		}
		break;

		case STATE_HEADLESS:
		{
			if (timer <= 0)
			{
				int pattern[] = {
					STATE_HOP,
					STATE_HOP,
					STATE_STOMP,
					-1
				};

				if (pattern[counter] == -1)
					counter = 0;

				switch (pattern[counter])
				{
					case STATE_HOP:
					{
						timer = 65;
						vsp = -1;
						hsp = 1 * ((hero.getX() > x)? 1: -1);
						aud_PlaySound(sounds[sndPi06]);
					}
					break;

					case STATE_STOMP:
					{
						state = STATE_STOMP;
						vsp = -4;
						onground = false;
						timer = 20*11;
					}
					break;
				}

				counter += 1;
			}
		}
		break;

		case STATE_DEATH:
		{
			y += 0.1;
			blinkTimer = timer;

			//create effects
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

	if (hsp)
	{
		x += hsp;

		Rectangle collide;
		if (mapCollision(x+32*sign(hsp), y, TILE_SOLID, &collide))
		{
			x = collide.x + 10 - (42 * sign(hsp));
		}
	}

	if (vsp)
	{
		y += vsp;

		if (vsp > 0)
		{
			Rectangle collide;
			if (mapCollision(x, y+32, TILE_SOLID, &collide))
			{
				y = collide.y - 32;
				vsp = 0;
				onground = true;

				aud_PlaySound(sounds[sndHit04]);
				gameSetQuake(30);
				effectList.add(new Dust(x-10,y+16,1));
				effectList.add(new Dust(x-10,y+16,-1));
			}
			else
			{
				onground = false;
			}
		}
		else
		{
			onground = false;
		}
	}

	if (!onground)
	{
		vsp += grav;
	}
	else
	{
		//slide to a stop
		if (hsp > 0)
		{
			if ((hsp -= fric) < 0)
				hsp = 0;
		}

		else if (hsp < 0)
		{
			if ((hsp += fric) > 0)
				hsp = 0;
		}
	}

	//update heads
	for (int i = 0; i < 4; i++)
		head[i].update();

	//animate
	if (onground)
		imageIndex = fmod(imageIndex + 0.1, 2);
	else
		imageIndex = ((vsp > 0)? 2: 3);

	//collisions
	Rectangle mask = (Rectangle){ (int)x-21, (int)y-10, 42, 42 };

	hero.hit(mask, 25);

	Object* wep = weaponList.collide(mask, this);

	if (wep)
	{
		hitTimer = 15;

		if (headless == false)
		{
			aud_PlaySound(sounds[sndPi05]);
			hsp = 0.75 * wep->getDirection();
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
				timer = 180;
				counter = 0;
				state = STATE_DEATH;
			}
		}
	}
}

void Hydra::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	if (!headless)
		dx += -2 * depth;

	//drew heads
	for (int i = 0; i < 4; i++)
		head[i].draw(subFrame, depth);

	int tile[] = { 5, 10 };

	gfx_DrawTile(images[imgBoss], dx-32, dy-32, 64, 64, tile[headless] + (int)imageIndex);
}


HydraHead::HydraHead(Object* owner, int position)
:Object(-100,-100)
{
	hp = 25;
	//hp = 1;
	state = STATE_HEAD_NORMAL;
	this->position = position;
	hitTimer = 0;
	blinkTimer = 0;
	timer = 0;
	counter = 0;
	imageIndex = 0;
	neckRot[0] = ((position == TOP_LEFT || position == TOP_RIGHT)? 0: 90);
	neckRot[1] = neckRot[0];

	for (int i = 0; i < 7; i++)
	{
		neckPosX[i] = this->x;
		neckPosY[i] = this->y;
	}

	this->owner = owner;

	if (owner == nullptr)
	{
		dead = true;
	}
	else
	{
		x = prevx = owner->getX();
		y = prevy = owner->getY();
	}
}

static int val1 = 205;
static int val2 = 40;
static int val3 = 30;

void HydraHead::update()
{
	Object::update();

	if (dead)
		return;

	x = owner->getX();
	y = owner->getY();

	if (timer > 0)
		timer -= 1;

	if (blinkTimer > 0)
		blinkTimer -= 1;

	//animate
	imageIndex = fmod(imageIndex + 0.1, 2);

	if (neckRot[0] > 360) neckRot[0] -= 360;

	neckRot[1] = neckRot[0];
	neckRot[0] += 2;

	switch (state)
	{
		case STATE_HEAD_NORMAL:
		{

		}
		break;

		case STATE_HEAD_FLAIL:
		{
			neckRot[0] += 4;

			if (timer <= 0)
				state = STATE_HEAD_NORMAL;
		}
		break;

		case STATE_HEAD_GOOP:
		{
			if (timer % 15 == 0)
			{
				float gx = neckPosX[6];
				float gy = neckPosY[6];
				enemyList.add(new HydraGoop(gx, gy));
				aud_PlaySound(sounds[sndPi06]);
			}

			if (timer <= 0)
				state = STATE_HEAD_NORMAL;
		}
		break;

		case STATE_HEAD_ELECTRIC:
		{
			if (timer % 40 == 0)
			{
				float ex = neckPosX[6];
				float ey = neckPosY[6];
				ex += 32 * ((position == TOP_LEFT || position == BOTTOM_LEFT)? -1: 1);
				ey += 10;
				enemyList.add(new HydraBolt(ex, ey));
				aud_PlaySound(sounds[sndShot03]);
			}

			if (timer % 40 > 20)
				neckRot[0] = neckRot[1];
			else
				neckRot[0] += 4;

			if (timer <= 0)
				state = STATE_HEAD_NORMAL;
		}
		break;

		case STATE_HEAD_DEATH:
		{
			if (timer % 6 == 0)
			{				
				float ex = neckPosX[6-counter];
				float ey = neckPosY[6-counter];
				aud_PlaySound(sounds[sndBom01]);
				effectList.add(new Poof(ex,ey-10));
				counter += 1;
			}

			if (counter >= 7)
			{
				dead = true;
			}

			return;
		}
		break;
	}

	updateNeck(x, y);

	//collisions
	for (int i = 6; i >= 0; i -= 2)
	{
		float mx = neckPosX[i];
		float my = neckPosY[i];

		Circle mask = (Circle){ (int)mx, (int)my, 10 };
		hero.hit(mask, 25);

		if (i == 6)
		{
			mask.r = 20;
			if (weaponList.collide(mask, this))
			{
				hp -= 1;
				hitTimer = 15;
				blinkTimer = hitTimer;

				if (hp > 0)
				{
					aud_PlaySound(sounds[sndHit02]);
				}
				else
				{
					timer = 120;
					counter = 0;
					state = STATE_HEAD_DEATH;
				}

				break;
			}
		}
	}
/*
	if (position == TOP_LEFT)
	{
		val1 += inp_ButtonPressed(BTN_R) - inp_ButtonPressed(BTN_L);
		val2 += inp_ButtonPressed(BTN_DOWN) - inp_ButtonPressed(BTN_UP);
		val3 += inp_ButtonPressed(BTN_RIGHT) - inp_ButtonPressed(BTN_LEFT);
	}
*/
}

void HydraHead::draw(float subFrame, float depth)
{
	if (dead || blinkTimer % 2)
		return;

	int dir = ((position == TOP_RIGHT || position == BOTTOM_RIGHT)? 1: -1);
	#ifndef _NO_LERP
	  float basex = lerp(x, prevx, subFrame);
	  float basey = lerp(y, prevy, subFrame);
	  float rot = lerp(neckRot[0], neckRot[1], subFrame);
	#endif
	float dx, dy;

	for (int i = 0; i < 7; i++)
	{
		if (state == STATE_HEAD_DEATH && counter > 6 - i)
			continue;

		#ifndef _NO_LERP
		  updateNeck(basex, basey, i, rot, &dx, &dy);
		#else
		  dx = neckPosX[i];
		  dy = neckPosY[i];
		#endif

		if (i == 6)
			gfx_DrawTile(images[imgBoss], dx-20+(-2*depth), dy-16, 40, 32, 4 * (dir == -1) + (int)imageIndex);
		else
			gfx_DrawTile(images[imgBoss], dx-16, dy-16, 32, 32, 10 + (dir == -1));				
	}
/*
	if (position == TOP_LEFT)
	{
		char str[16];
		sprintf(str, "%d", val1);
		drawTextBold(str, 10, 50);
		sprintf(str, "%d", val2);
		drawTextBold(str, 10, 70);	
		sprintf(str, "%d", val3);
		drawTextBold(str, 10, 90);	
	}
*/
}

void HydraHead::updateNeck(float inx, float iny, int index, float rot, float* outx, float* outy)
{
	float workx = inx - 10;
	float worky = iny - 5;
	float offset = val1;
	float ang;

	if (index == -1)
		rot = neckRot[0];

	if (position == TOP_RIGHT || position == TOP_LEFT)
		offset = 245;
	else
		worky += 10;

	for (int i = 0; i < 7; i++)
	{
		rot += val2;
		ang = lengthdir_x(val3, rot);
		workx += lengthdir_x(12, offset + ang);
		worky += lengthdir_y(12, offset + ang);

		float tempx = workx;

		if (position == TOP_RIGHT || position == BOTTOM_RIGHT)
			tempx = inx - (tempx - inx);

		if (index == -1)
		{
			neckPosX[i] = tempx;
			neckPosY[i] = worky;
		}
		else
		{
			if (i == index)
			{
				*outx = tempx;
				*outy = worky;
				break;
			}
		}
	}
}

void HydraHead::setState(int state, int timer)
{
	if (this->state != STATE_HEAD_DEATH)
	{
		this->state = state;
		this->timer = timer;
	}
}


HydraGoop::HydraGoop(float x, float y)
:Object(x,y)
{
	bounce = false;
	hsp = -2 + (rand() % 5);
	vsp = -3;
}

void HydraGoop::update()
{
	Object::update();

	x += hsp;
	y += (vsp += 0.1);

	//bounce
	if (!bounce)
	{
		if (mapCollision(x,y+10,TILE_SOLID) && !mapCollision(x,y-10,TILE_SOLID))
		{
			vsp = -1;
			bounce = true;
		}
	}

	//collisions
	Circle mask = (Circle){ (int)x, (int)y, 8 };

	if (collision(mask, hero.getShieldMask()))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	else if (hero.hit(mask, 25))
	{
		hero.poison();
	}

	//out of room
	if (x < -10 || x > 330 || y > 250)
		dead = true;
}

void HydraGoop::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	dx += -2 * depth;

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 200 + (int)goopImage);
}


HydraRock::HydraRock()
:Object(0,-16)
{
	x = prevx = 40 + (rand() % 240);
	bounce = false;
	vsp = 0;
	imageIndex = 0;
}

void HydraRock::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.25, 8);

	y += (vsp += 0.075);

	//bounce
	if (!bounce)
	{
		if (mapCollision(x,y+10,TILE_SOLID) && !mapCollision(x,y-10,TILE_SOLID))
		{
			aud_PlaySound(sounds[sndHit06]);
			vsp = -1;
			bounce = true;
		}
	}

	//collisions
	Rectangle mask = (Rectangle){ (int)x-10, (int)y-10, 20, 20 };

	hero.hit(mask, 30);

	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;
		aud_PlaySound(sounds[sndHit03]);
	}

	//out of room
	if (y > 260)
		dead = true;
}

void HydraRock::draw(float subFrame, float depth)
{
	float dx = x;
	float dy = lerp(y, prevy, subFrame);

	dx += -2 * depth;

	gfx_DrawTile(images[imgChr32], dx-16, dy-16, 32, 32, 22 + (int)imageIndex);
}


HydraBolt::HydraBolt(float x, float y)
:Object(x,y)
{
	state = 0;
	timer = 20;
	hsp = 0;
	vsp = 0;
}

void HydraBolt::update()
{
	Object::update();

	if (state == 0)
	{
		if ((timer -= 1) <= 0)
		{
			state = 1;
			float ang = angle(x, y, hero.getX(), hero.getY());
			hsp = lengthdir_x(2.5, ang);
			vsp = lengthdir_y(2.5, ang);
		}
	}

	x += hsp;
	y += vsp;

	//collisions
	Circle mask = (Circle){ (int)x, (int)y, 10 };

	if (hero.hit(mask, 25))
		hero.stun();

	//out of room
	if (x < -20 || x > 340 || y < -20 || y > 260)
		dead = true;
}

void HydraBolt::draw(float subFrame, float depth)
{
	if (state == 0 && timer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	dx += -2 * depth;
	
	gfx_DrawTile(images[imgChr32], dx-16, dy-16, 32, 32, 30 + (int)boltImage);
}