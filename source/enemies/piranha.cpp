#include "piranha.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

#define fric (0.01f)

enum {
	STATE_NORMAL,
	STATE_TURN
};

Piranha::Piranha(float x, float y, bool flip)
:Object(x,y)
{
	imageIndex = 0;
	direction = 1;
	speed = 0.5;
	state = 0;
	xstart = this->x;

	if (flip)
	{
		direction = -1;
		speed = -0.5;
	}
}

void Piranha::update()
{
	Object::update();

	x += speed;

	if (direction == 1)
	{
		if (x > xstart + 12)
		{
			if ((speed -= fric) < 0)
			{
				direction = -1;
				state = STATE_TURN;
				imageIndex = 0;
			}
		}
		else
		{
			if ((speed += fric) > 0.5)
				speed = 0.5;
		}
	}
	else if (direction == -1)
	{
		if (x < xstart - 12)
		{
			if ((speed += fric) > 0)
			{
				direction = 1;
				state = STATE_TURN;
				imageIndex = 0;
			}
		}
		else
		{
			if ((speed -= fric) < -0.5)
				speed = -0.5;
		}
	}

	//animate
	if (state == STATE_NORMAL)
	{
		imageIndex = fmod(imageIndex + 0.1, 2);
	}
	else if (state == STATE_TURN)
	{
		if ((imageIndex += 0.25) >= 3)
		{
			state = STATE_NORMAL;
			imageIndex = 0;			
		}
	}

	//collision
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+3, 16, 16 };
	
	hero.hit(mask, 15);
	
	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Piranha::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);

	int tile = 153;

	if (state == STATE_NORMAL)
	{
		tile = 153 + ((direction == -1)? 2: 0) + (int)imageIndex;
	}
	else if (state == STATE_TURN)
	{
		if (direction == 1)
		{
			int animation[] = { 1, 2, 0 };
			tile = 157 + animation[(int)imageIndex];
		}
		else
		{
			int animation[] = { 0, 2, 1 };
			tile = 157 + animation[(int)imageIndex];
		}
	}

	gfx_DrawTile(images[imgEne01], dx, y, 20, 20, tile);
}