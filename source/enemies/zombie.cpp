#include "zombie.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	TYPE_NORMAL,
	TYPE_POISON
};

enum {
	STATE_WAIT,
	STATE_POPUP,
	STATE_WALK
};

Zombie::Zombie(float x, float y, int type)
:Object(x,y)
{
	hp = 2;
	this->type = type;
	state = STATE_WAIT;
	direction = 1;
	vsp = 0;
	imageIndex = 0;
	onGround = false;
}

static Rectangle _getMask(float x, float y)
{
	return (Rectangle){ (int)x+4, (int)y+4, 12, 16 };
}

void Zombie::update()
{
	Object::update();

	switch (state)
	{
		case STATE_WAIT:
		{
			Rectangle area = (Rectangle) { (int)x - 60, (int)y - 10, 140, 40 };
			if (collision(hero.getX(), hero.getY(), area))
			{
				state = STATE_POPUP;
				imageIndex = 0;
			}

			return;
		}
		break;

		case STATE_POPUP:
		{
			if ((imageIndex += 0.15) >= 4)
			{
				state = STATE_WALK;
				vsp = -0.5;
				imageIndex = 0;
				aud_PlaySound(sounds[sndPi05]);
				direction = ((hero.getX() > x)? 1: -1);
			}
		}
		break;

		case STATE_WALK:
		{
			//animate
			imageIndex = fmod(imageIndex += 0.1, 2);

			//vertical movement
			if (onGround == false)
			{
				y += (vsp += 0.05);

				Rectangle collide;
				if (solidCollision(_getMask(x,y), &collide))
				{
					vsp = 0;
					onGround = true;
					y = collide.y - 20;
				}
			}

			//horizontal movement
			float hsp = 0.5 * direction;
			
			if ((int)imageIndex == 0)
				hsp *= 0.5;
			
			if (type == TYPE_POISON)
				hsp *= 2;

			x += hsp;

			//touch wall / turn on ledge
			if (solidCollision(x+10+8*direction, y+10) || (onGround && !solidCollision(x+10+8*direction, y+30)))
				direction *= -1;
		}
		break;
	}

	Rectangle mask = _getMask(x,y);

	if (hero.hit(mask, 10) && type == TYPE_POISON)
		hero.poison();

	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

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

void Zombie::draw(float subFrame, float depth)
{
	if (state == STATE_WAIT || hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 64;

	if (type == TYPE_POISON)
		tile += 64;

	if (state == STATE_POPUP)
		tile += 4;
	else if (direction == -1)
		tile += 2;

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, tile + (int)imageIndex);
}