#include "bee.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	STATE_HOVER,
	STATE_BACKWARDS,
	STATE_DOWNWARDS,
	STATE_DIAGONAL,
	STATE_RETURN
};

Bee::Bee(float x, float y, bool flip)
:Object(x,y)
{
	state = STATE_HOVER;
	timer = 0;
	direction = ((flip)? -1: 1);
	imageIndex = 0;
	hsp = 0;
	vsp = 0;
	xstart = this->x;
	ystart = this->y;
	hoverRot = ((flip)? 0: 180);
}

void Bee::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.3, 3);

	switch (state)
	{
		case STATE_HOVER:
		{
			hoverRot = fmod(hoverRot + 2.6, 360);
			x = xstart + lengthdir_x(10, hoverRot);

			direction = ((hoverRot > 180)? 1: -1);

			//check if player is close
			Rectangle area = (Rectangle){ (int)x-40, (int)y, 100, 50 };
			if (collision(hero.getX(), hero.getY(), area))
			{
				aud_PlaySound(sounds[sndBee01]);
				state = STATE_BACKWARDS;
				direction = ((hero.getX() > x+10)? 1: -1);
				hsp = -2.75 * direction;
			}
		}
		break;

		case STATE_BACKWARDS:
		{
			hsp += 0.125 * direction;

			if (sign(hsp) == direction)
			{
				state = STATE_DOWNWARDS;
				hsp = 0;
				vsp = 1.875f;
			}
		}
		break;

		case STATE_DOWNWARDS:
		{
			vsp -= 0.05f;

			if (vsp <= 0)
			{
				state = STATE_DIAGONAL;
				vsp = 0;
				direction = ((hero.getX() > x+10)? 1: -1);
				hsp = 1.5 * direction;
			}
		}
		break;

		case STATE_DIAGONAL:
		{
			if ((vsp -= 0.05) < -1.5)
				vsp = -1.5;

			if (y <= ystart)
			{
				state = STATE_RETURN;
				y = ystart;
				vsp = 0;
				direction = ((x > xstart)? -1: 1);
				hsp = 0.5 * direction;
			}
		}
		break;

		case STATE_RETURN:
		{
			if (sign(x - xstart) == direction)
			{
				state = STATE_HOVER;
				x = xstart;
				hsp = 0;
				hoverRot = ((direction == 1)? 270: 90);
			}
		}
		break;
	}

	//movement
	x += hsp;
	y += vsp;

	//collisions
	Rectangle mask = (Rectangle){ (int)x+7, (int)y+3, 12, 16 };
	if (direction == -1)
		mask.x = x + 1;

	hero.hit(mask, 15);

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Bee::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 199 + (int)imageIndex + ((direction == 1)? 0: 3);

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, tile);
}