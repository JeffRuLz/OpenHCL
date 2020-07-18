#include "statue.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../math.hpp"
#include "../map.hpp"
#include "../objects/collectable.hpp"
#include "../effects/tink.hpp"
#include "../effects/brokenrock.hpp"

enum {
	TYPE_RHYNO = 0,
	TYPE_DRAGON = 1,
	TYPE_MEDUSA = 2,
	TYPE_DEMON = 3,
	TYPE_FIREBALL = 4,
	TYPE_JAR = 5
};

enum {
	STATE_WAIT,
	STATE_BLINK
};

Statue::Statue(float x, float y, int type, bool flip, int offset, int timer)
:Object(x,y)
{
	imageIndex = 0;
	hp = 5;
	this->timer = offset;
	this->type = type;
	state = STATE_WAIT;
	direction = (flip == true)? -1: 1;
	this->waitTime = timer;
	counter = 0;

	if (this->waitTime == 0)
		this->waitTime = 60;

	if (type == TYPE_RHYNO)
	{
		this->waitTime = 120;
		layer = 1;
	}

	if (type == TYPE_DRAGON)
		this->waitTime += 80;

	if (type == TYPE_MEDUSA)
	{
		layer = 1;
	}

	if (type == TYPE_FIREBALL)
	{
		layer = 0;
	}
}

void Statue::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	switch (state)
	{
		case STATE_WAIT:
		{
			if (timer <= 0)
			{
				switch (type)
				{
					case TYPE_RHYNO:
					{
						Rectangle area = (Rectangle){ (int)x, (int)y-20, 180, 50 };
				
						if (direction == -1)
							area.x -= area.w - 20;

						if (collision(hero.getX(), hero.getY(), area))
						{
							state = STATE_BLINK;
							timer = 30;
						}
					}
					break;

					case TYPE_DEMON:
					{
						Rectangle area = (Rectangle){ (int)x, (int)y-10, 200, 40 };
						
						if (direction == -1)
							area.x -= area.w - 20;

						if (collision(hero.getX(), hero.getY(), area))
						{
							state = STATE_BLINK;
							timer = 30;
						}
					}
					break;

					default:
					{
						state = STATE_BLINK;
						timer = 30;
					}
				}
				
			}
		}
		break;

		case STATE_BLINK:
		{
			if (timer <= 0)
			{
				//shoot
				switch (type)
				{
					case TYPE_RHYNO:
					{
						aud_PlaySound(sounds[sndShot03]);
						enemyList.add(new Bullet(x+10,y+10,direction));
					}
					break;

					case TYPE_DRAGON:
					{
						aud_PlaySound(sounds[sndFire01]);
						enemyList.add(new Fire(x+20-80*(direction == -1), y-5, direction));
					}
					break;

					case TYPE_MEDUSA:
					{
						aud_PlaySound(sounds[sndShot04]);
						enemyList.add(new Laser(x+10,y+10,direction));
					}
					break;

					case TYPE_DEMON:
					{
						enemyList.add(new Boulder(x+10,y+10,direction));
					}
					break;

					case TYPE_FIREBALL:
					{
						aud_PlaySound(sounds[sndShot03]);
						float ang = angle(x+10,y+10,hero.getX(),hero.getY());
						enemyList.add(new EnemyFireBall(x+10,y+10,ang,1.5));
					}
					break;

					case TYPE_JAR:
					{
						if (counter == 0)
							aud_PlaySound(sounds[sndShot06]);
						enemyList.add(new Air(x,y));

						if ((counter += 1) < 6)
							timer = 10;
					}
					break;
				}

				if (timer == 0)
				{
					state = STATE_WAIT;
					timer = waitTime;
					counter = 0;
				}
			}
		}
		break;
	}

	//hero collision
	Rectangle mask = { (int)x + 6, (int)y + 4, 8, 16 };

	if (type != TYPE_JAR || !gameHasItem(ITEM_ROC_FEATHER))
		hero.hit(mask, 10);

	//weapons
	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
			aud_PlaySound(sounds[sndHit02]);
		else
		{
			aud_PlaySound(sounds[sndBom02]);
			rockSmashEffect(x+10, y+10);
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void Statue::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = x;
	int tile[] = { 32, 36, 40, 54, 14, 58 };
	int offset[] = { -3, 0, -1, 0, 0, 0 };
	int frames[] = { 2, 2, 2, 2, 1, 1 };
	int t = tile[type] + (direction == -1);

	//flash white
	if (state == STATE_BLINK && timer % 6 < 3)
		t += frames[type];

	dx += offset[type] * direction;

	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, t);
}



Bullet::Bullet(float x, float y, int direction)
:Object(x,y)
{
	this->direction = direction;
	imageIndex = 0;
	layer = 0;
}

void Bullet::update()
{
	Object::update();

	//animate
	imageIndex += 0.3 * direction;
	if (imageIndex < 0) imageIndex += 4;
	if (imageIndex >= 4) imageIndex -= 4;

	//movement
	x += 2 * direction;

	//collide with wall
	Rectangle mask = (Rectangle){ (int)x-3, (int)y-3, 6, 6 };
	if (solidCollision(mask))
	{
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//shield
	else if (collision(mask, hero.getShieldMask()))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//collide with hero
	else
	{
		hero.hit(mask, 10);
	}	

	//outside of room
	if (x < -10 || x > 330)
		dead = true;
}

void Bullet::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, 196 + (int)imageIndex);
}


Laser::Laser(float x, float y, int direction)
:Object(x,y)
{
	this->direction = direction;
	imageIndex = 0;
	layer = 0;
}

void Laser::update()
{
	Object::update();

	//movement
	x += 5 * direction;

	//animate
	imageIndex = fmod(imageIndex + 0.3, 2);

	//collisions
	Rectangle mask = (Rectangle){ (int)x-10, (int)y-2, 20, 4 };

	//wall
	if (solidCollision(mask))
	{
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//shield
	else if (collision(mask, hero.getShieldMask()))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//hero
	else if (hero.hit(mask, 15))
	{
		hero.petrify();
		dead = true;
	}

	//outside of room
	if (x < -10 || x > 330)
		dead = true;
}

void Laser::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	
	int tile = 192 + ((direction < 0)? 2: 0);

	gfx_DrawTile(images[imgChr20], dx-10, y-10, 20, 20, tile + (int)imageIndex);
}


EnemyFireBall::EnemyFireBall(float x, float y, float ang, float speed)
:Object(x,y)
{
	hsp = lengthdir_x(speed, ang);
	vsp = lengthdir_y(speed, ang);
	imageIndex = 0;
	layer = 1;
}

void EnemyFireBall::update()
{
	Object::update();

	//movement
	x += hsp;
	y += vsp;

	//animate
	imageIndex = fmod(imageIndex + 0.5, 8);

	//collision
	Circle mask = (Circle){ (int)x, (int)y, 7 };

	//shield
	if (collision(mask, hero.getShieldMask()))
	{
		aud_PlaySound(sounds[sndHit07]);
		effectList.add(new Tink(x,y));
		dead = true;
	}
	//hero
	else
	{
		hero.hit(mask, 10);
	}

	//destroy outside of room
	if (x < -10 || x > 330 || y < -10 || y > 250)
		dead = true;
}

void EnemyFireBall::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 184 + (int)imageIndex);
}


Air::Air(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
}

void Air::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.5, 2);

	//movement
	y -= 3;

	//collisions
	if (hero.getState() != HERO_STATE_HIT)
	{
		Rectangle mask = (Rectangle){ (int)x+1, (int)y+1, 18, 15 };
		
		if (!gameHasItem(ITEM_ROC_FEATHER))
		{
			hero.hit(mask, 10);
		}
		else if (collision(mask, hero.getMask()))
		{
			if (hero.getVsp() > -2.5)
			{
				hero.setVsp(-2.5);
				hero.setOnGround(false);
			}
		}
	}

	//destroy
	if (y < -10)
		dead = true;
}

void Air::draw(float subFrame, float depth)
{
	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgChr20], x, dy, 20, 20, 224 + (int)imageIndex);
}


Boulder::Boulder(float x, float y, int direction)
:Object(x,y)
{
	imageIndex = 0;
	hsp = 1.5 * direction;
	vsp = 0;
}

void Boulder::update()
{
	Object::update();

	//animate
	imageIndex += 0.25 * sign(hsp);
	if (imageIndex >= 8) imageIndex -= 8;
	if (imageIndex < 0) imageIndex += 8;

	//collide with floor
	y += (vsp += 0.06);

	Rectangle collide;
	if (solidCollision(x, y+8, &collide))
	{
		aud_PlaySound(sounds[sndHit06]);
		vsp = -1.6;
		y = collide.y - 8;
	}

	x += hsp;
	Rectangle mask = (Rectangle){ (int)x-8, (int)y-8, 16, 16 };

	hero.hit(mask, 20);

	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;
		aud_PlaySound(sounds[sndHit03]);
	}

	//collide with wall
	if (solidCollision(mask))
	{
		aud_PlaySound(sounds[sndBom02]);
		rockSmashEffect(x,y);
		dead = true;
	}
}

void Boulder::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgEne01], dx-10, dy-10, 20, 20, 72 + (int)imageIndex);
}


Fire::Fire(float x, float y, int direction)
:Object(x,y)
{
	timer = 60;
	this->direction = direction;
	imageIndex = 0;
}

void Fire::update()
{
	//animate
	imageIndex += 0.25;

	if (timer > 0)
	{
		imageIndex = fmod(imageIndex, 3);

		if ((timer -= 1) <= 0)
			imageIndex = 3;
	}

	if (timer <= 0 && imageIndex >= 6)
		dead = true;

	Rectangle mask = (Rectangle){ (int)x+5, (int)y+12, 50, 5 };
	HeroState hs = hero.getState();
	if (hero.hit(mask, 30))
	{
		if (hs == HERO_STATE_LADDER)
		{
			hero.setHsp(0);
		}

		//force hero to fly back in flame's direction
		if (hero.getHsp() != 0)
		{
			hero.setHsp(abs(hero.getHsp()) * direction);
			hero.setDirection(-direction);
		}
	}
}

void Fire::draw(float subFrame, float depth)
{
	gfx_DrawTile(images[imgChr6020], x, y, 60, 20, (int)imageIndex + 6 * (direction == -1));
}