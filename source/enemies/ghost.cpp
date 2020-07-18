#include "ghost.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../objects/collectable.hpp"
#include "../effects/poof.hpp"

enum {
	STATE_WAIT,
	STATE_CHASE
};

Ghost::Ghost(float x, float y)
:Object(x,y)
{
	state = STATE_WAIT;
	timer = 0;
	ang = 0;
	imageIndex = 0;
	rot = 0;
	yoffset[0] = 0;
	yoffset[1] = 0;

	layer = 1;
}

void Ghost::update()
{
	Object::update();

	switch (state)
	{
		case STATE_WAIT:
		{
			if ((timer -= 1) <= 0)
			{
				Rectangle area = (Rectangle){ (int)x-40, (int)y-40, 100, 100 };

				if (collision(hero.getX(), hero.getY(), area))
				{
					aud_PlaySound(sounds[sndPi08]);
					state = STATE_CHASE;
					ang = (rand() % 360);
					timer = 130;
				}
			}
		}
		break;

		case STATE_CHASE:
		{
			x += lengthdir_x(1, ang);
			y += lengthdir_y(1, ang);

			float targetAngle = ang - angle(x+10,y+10,hero.getX(),hero.getY());
			if (targetAngle < 0) targetAngle += 360;
			if (targetAngle > 360) targetAngle -= 360;

			if (targetAngle < 180)
				ang -= 2;
			else
				ang += 2;

			if ((timer -= 1) <= 0)
			{
				state = STATE_WAIT;
				timer = 10;
			}
		}
		break;
	}

	//animate
	float imgSpd[] = { 0.2, 0.3 };
	imageIndex = fmod(imageIndex + imgSpd[state], 4);

	//hover
	rot = (rot + 5) % 360;
	yoffset[1] = yoffset[0];
	yoffset[0] = lengthdir_y(2.5, rot);

	//collision
	Circle mask = (Circle){ (int)x+10, (int)y+10, 5 };
	hero.hit(mask, 10);

	mask.r = 10;
	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Ghost::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y + yoffset[0], prevy + yoffset[1], subFrame);

	dx += (-2 - lengthdir_x(2, rot)) * depth;

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, 28 + (int)imageIndex);
}