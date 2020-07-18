#include "jelly.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../map.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_IDLE,
	STATE_JUMP
};

enum {
	TYPE_BLUE = 0,
	TYPE_RED = 1,
	TYPE_YELLOW = 2
};

static const float grav = 0.0625;

Jelly::Jelly(float x, float y, int type, int delay)
:Object(x,y)
{
	if (type < 0) type = 0;
	if (type > 3) type = 3;

	hp = 1;
	hsp = 0;
	vsp = 0;
	imageIndex = 0;
	this->type = type;
	this->timer = delay;
	counter = 0;
	state = STATE_IDLE;
	onground = false;
}

void Jelly::update()
{
	Object::update();

	if (timer > 0) timer -= 1;

	switch (state)
	{
		case STATE_IDLE:
		{
			if ((imageIndex += 0.25) >= 6.f)
			{
				imageIndex = 0.f;

				if (timer <= 0)
				{
					state = STATE_JUMP;

					//hop sideways
					float hspeed[] = { 0.0f, 0.5f, 0.75 };
					hsp = hspeed[type];

					//random direction
					if (rand() % 2)
						hsp *= -1;

					//jump
					counter += 1;
					vsp = -1;
					onground = false;

					if (counter >= 3)
					{
						vsp = -2;
						counter = 0;
					}
				}
			}
		}
		break;

		case STATE_JUMP:
		{
			//animate
			imageIndex = 5 + (vsp >= 0);

			//horizontal movement
			if (hsp)
			{
				x += hsp;

				//stay within room
				if (x > 310) x = 310;
				if (x < -10) x = -10;

				//collide with wall
				Rectangle mask = { (int)x+4, (int)y+2, 12, 12 };
				Rectangle collide;
				if (solidCollision(mask, &collide))
				{
					//left
					if (hsp < 0)
						x = collide.x + 20 - 4;
					//right
					else
						x = collide.x - 20 + 4;
				}
			}
		}
		break;

		default:
		{
			state = STATE_IDLE;
		}
	}

	//vertical movement
	if (vsp)
	{
		y += vsp;

		Rectangle mask = { (int)x + 4, (int)y + 2, 12, 12 };
		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			//floor
			if (vsp >= 0)
			{
				y = collide.y - 20 + 6;
				vsp = 0;
				onground = true;

				if (state == STATE_JUMP)
				{
					state = STATE_IDLE;
					imageIndex = 0;
					hsp = 0;
				}
			}
			//ceiling
			else
			{
				y = collide.y + 20 - 2;
			}
		}
	}

	//gravity
	if (onground == false)
		vsp += grav;

	//player collision
	Circle mask = { (int)x+10, (int)y+10, 6 };
	int damage[] = { 10, 20, 20 };

	if (hero.hit(mask, damage[type]))
	{
		if (type == TYPE_YELLOW)
			hero.stun();
	}

	//weapon collision
	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}

	//outside of room
	if (y > 240)
		dead = true;
}

void Jelly::draw(float subFrame, float depth)
{
	float drawx = lerp(x, prevx, subFrame);
	float drawy = lerp(y, prevy, subFrame);

	int tile[] = { 0, 7, 192 };
	int frame = (int)imageIndex;

	if (state == STATE_IDLE)
	{
		int animation[] = { 0, 1, 2, 3, 4, 3 };
		frame = animation[frame % 6];
	}

	gfx_DrawTile(images[imgEne01], drawx, drawy, 20, 20, tile[type] + frame);
}