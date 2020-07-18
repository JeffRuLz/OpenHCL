#include "knight.hpp"
#include "../math.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"
#include "../effects/poof.hpp"
#include "../effects/sparkle.hpp"
#include "../objects/collectable.hpp"

enum {
	TYPE_SWORD = 0,
	TYPE_SHIELD = 1,
	TYPE_BOOM = 2,
	TYPE_POISON = 3,
	TYPE_PUMPKIN = 4
};

enum {
	STATE_WALK,
	STATE_WAIT,
	STATE_SHOOT
};

static inline int _randomTimer()
{
	return 60 + (rand() % 6) * 60;
}

Knight::Knight(float x, float y, char type)
:Object(x,y)
{
	hp = 2;

	this->type = type;
	state = STATE_WALK;
	imageIndex = 0;
	timer = _randomTimer();
	blinkTimer = 0;
	direction = ((hero.getX() > this->x+10)? 1: -1);

	if (type >= TYPE_BOOM)
		timer = 0;
}

void Knight::update()
{
	Object::update();

	if (blinkTimer > 0)
		blinkTimer -= 1;

	switch (state)
	{
		case STATE_WALK:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.1, 2);

			//movement
			float speed[] = { 0.5, 0.25, 0.5, 0.5, 0.5 };
			x += speed[type] * direction;

			//turn at walls / ledges
			if (solidCollision(x+10+10*direction, y+10) || !solidCollision(x+10+10*direction, y+30))
				direction *= -1;

			if (timer > 0)
			{
				timer -= 1;
			}
			else
			{
				//stop walking
				if (type <= TYPE_SHIELD)
				{					
					state = STATE_WAIT;
					timer = 120;
				}

				//detect hero
				else
				{
					Rectangle area = (Rectangle){ (int)x-60, (int)y, 140, 20 };
					if (collision(hero.getX(), hero.getY(), area))
					{
						direction = ((hero.getX() > x+10)? 1: -1);
						imageIndex = 1;
						state = STATE_SHOOT;
						timer = -15;
					}
				}
			}
		}
		break;

		case STATE_WAIT:
		{
			imageIndex = 0;

			if (timer-- <= 0)
			{
				state = STATE_WALK;
				direction = ((hero.getX() > this->x+10)? 1: -1);
				timer = _randomTimer();
			}
		}
		break;

		case STATE_SHOOT:
		{
			//small delay before shoot
			if (timer < 0)
			{
				if ((timer += 1) >= 0)
				{
					imageIndex = 2;

					switch (type)
					{
						case TYPE_POISON:
						{
							timer = 30;
							aud_PlaySound(sounds[sndPi05]);
							enemyList.add(new Goop(x+10,y+10,direction));
						}
						break;

						case TYPE_BOOM:
						{
							timer = 110;
							aud_PlaySound(sounds[sndPi05]);
							enemyList.add(new EnemyBoomerang(x+10,y+10,direction));
						}
						break;

						case TYPE_PUMPKIN:
						{
							timer = 30;
							aud_PlaySound(sounds[sndPi05]);
							enemyList.add(new Pumpkin(x+10,y+6,direction));
						}
						break;
					}
				}
			}
			//after shoot
			else if (timer > 0)
			{
				if ((timer -= 1) <= 0)
				{
					state = STATE_WALK;
					timer = 120;
				}
			}
		}
		break;
	}

	Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 14, 16 };

	if (!hero.hit(mask, 15) && type == TYPE_SWORD)
	{
		//sword collision
		Rectangle swordMask = { (int)x + (12*direction), (int)y + 10, 20, 5 };
		hero.hit(swordMask, 30);
	}

	Object* wep = weaponList.collide(mask, this);
	if (wep)
	{
		hitTimer = 15;

		if (type == TYPE_SHIELD && direction == -(wep->getDirection()))
		{
			aud_PlaySound(sounds[sndHit03]);
		}
		else
		{
			hp -= 1;
			blinkTimer = 15;

			if (hp > 0)
				aud_PlaySound(sounds[sndHit02]);
			else
			{
				aud_PlaySound(sounds[sndBom01]);
				effectList.add(new Poof(x+10,y));
				dropCollectable(x,y);
				dead = true;
			}
		}
	}
}

void Knight::draw(float subFrame, float depth)
{
	if (blinkTimer % 2)
		return;
	
	float dx = lerp(x, prevx, subFrame);

	if (type == TYPE_SWORD)
	{
		//draw sword
		int tile = 84 + (direction == -1);
		float sx = dx + (12 * direction);
		float sy = y + 4;

		if ((int)imageIndex == 1)
		{
			sx -= direction;
			sy -= 1;
		}

		gfx_DrawTile(images[imgEne01], sx, sy, 20, 20, tile);
	}

	//draw body
	int tile[] = { 80, 86, 170, 112, 224 };
	int frames[] = { 2, 2, 3, 3, 3 };
	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, tile[type] + (int)imageIndex + (frames[type] * (direction == -1)) );
}


Goop::Goop(float x, float y, int direction)
:Object(x,y)
{
	hsp = 2 * direction;
	imageIndex = 0;
}

void Goop::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 3);

	//movement
	x += hsp;

	Rectangle mask = (Rectangle){ (int)x-5, (int)y-5, 10, 10 };

	//collide with wall
	if (solidCollision(mask))
	{
		effectList.add(new Tink(x,y));
		dead = true;
	}

	//outside of room
	else if (x < -20 || x > 340)
	{
		dead = true;
	}

	//collide with shield
	else if (collision(mask, hero.getShieldMask()))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}

	//collide with hero
	else if (hero.hit(mask, 10))
	{
		hero.poison();
	}
}

void Goop::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	int tile = 218 + (3 * (hsp < 0)) + (int)imageIndex;

	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, tile);
}


EnemyBoomerang::EnemyBoomerang(float x, float y, int direction)
:Object(x,y)
{
	this->direction = ((direction == 1)? 1: -1);
	xstart = this->x;
	hsp = 3;
	imageIndex = 0;
}

void EnemyBoomerang::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 8);

	//movement
	x += hsp * direction;
	hsp -= 0.0625;

	//collisions
	Rectangle mask = (Rectangle){ (int)x+4, (int)y+4, 12, 12 };
	hero.hit(mask, 10);

	//finish
	if (hsp < 0 && ((direction == 1 && x < xstart+20) || (direction == -1 && x > xstart-20)))
	{
		effectList.add(new Sparkle(x,y));
		dead = true;
	}
}

void EnemyBoomerang::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, 144 + (int)imageIndex);
}


Pumpkin::Pumpkin(float x, float y, int direction)
:Object(x,y)
{
	explosion = false;
	hsp = 1.5 * ((direction == 1)? 1: -1);
	vsp = -1;
	imageIndex = 0;
}

void Pumpkin::update()
{
	Object::update();

	//pumpkin head
	if (!explosion)
	{
		imageIndex = fmod(imageIndex + 0.1, 2);

		{
			x += hsp;

			Rectangle mask = (Rectangle){ (int)x-5, (int)y-5, 10, 10 };
			Rectangle collide;

			if (solidCollision(mask, &collide))
			{
				x = collide.x + 10 - (15 * sign(hsp));
				hsp *= -1;
			}
		}

		{
			y += vsp;
			vsp += 0.075;

			Rectangle mask = (Rectangle){ (int)x-5, (int)y-5, 10, 10 };
			Rectangle collide;

			if (solidCollision(mask, &collide))
			{
				if (vsp < 0)
				{
					y = collide.y + 25;
				}
				else
				{
					y = collide.y - 18;
					hsp = 0;
					vsp = 0;
					aud_PlaySound(sounds[sndBom03]);
					explosion = true;
					imageIndex = 0;
				}
			}
		}
	}

	//explosion
	else
	{
		if ((imageIndex += 0.3) >= 12)
			dead = true;

		Rectangle mask = (Rectangle){ (int)x-17, (int)y-17, 34, 34 };
		hero.hit(mask, 40);
	}

	//destroy out of room
	if (y > 250)
		dead = true;
}

void Pumpkin::draw(float subFrame, float depth)
{
	//pumpkin head
	if (!explosion)
	{
		float dx = lerp(x, prevx, subFrame);
		float dy = lerp(y, prevy, subFrame);
		int tile = 96 + (int)imageIndex + (2 * (sign(hsp) == -1));
		gfx_DrawTile(images[imgEne01], dx-10, dy-10, 20, 20, tile);
	}

	//explosion
	else
	{
		gfx_DrawTile(images[imgChr64], x-32, y-24, 64, 48, (int)imageIndex);
	}
}