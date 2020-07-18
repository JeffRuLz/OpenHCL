#include "bat.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

enum {
	TYPE_GRAY,
	TYPE_RED
};

enum {
	STATE_WAIT,
	STATE_FLY
};

Bat::Bat(float x, float y, char type)
:Object(x,y)
{
	xstart = this->x;
	ystart = this->y;
	imageIndex = 5;
	state = STATE_WAIT;
	this->type = type;
	direction = 1;
	flyback = false;
	timer = 0;
	angle = 0;	
}

void Bat::update()
{
	Object::update();

	switch (state)
	{
		case STATE_WAIT:
		{
			//animate
			imageIndex = 5;

			//wait for hero to get near
			if (timer > 0)
				timer -= 1;

			if (timer <= 0)
			{
				Rectangle area = (Rectangle) { (int)x - 20, (int)y, 60, 50 };

				if (type == TYPE_RED)
				{
					area.x -= 10;
					area.w += 20;
				}

				if (collision(hero.getX(), hero.getY(), area))
				{
					aud_PlaySound(sounds[sndPi07]);

					state = STATE_FLY;
					imageIndex = 0;
					angle = 270;
					flyback = true;

					if (hero.getX() > x + 10)
						direction = 1;
					else
						direction = -1;
				}
			}
		}
		break;

		case STATE_FLY:
		{
			//animate
			imageIndex = fmod(imageIndex + 0.25, 5);

			//y bob
			angle = (angle + 4) % 360;
			y = ystart + 15 + lengthdir_y(15, angle);

			//horizontal movement
			if (type == TYPE_RED)
				x += direction;

			//vertical bob finish
			if (angle == 270)
			{
				if (type == TYPE_RED && flyback)
				{
					direction *= -1;
					flyback = false;
				}
				else
				{
					state = STATE_WAIT;
					timer = 70;
					setX(xstart);
				}
			}
		}
		break;
	}

	Rectangle mask = (Rectangle){ (int)x+2, (int)y, 16, 14 };

	hero.hit(mask, 10);

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Bat::draw(float subFrame, float depth)
{
	int tile[] = { 48, 122 };

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgEne01], dx, dy-2, 20, 20, tile[(int)type] + (int)imageIndex);
}