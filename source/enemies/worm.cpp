#include "worm.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

#define grav 0.1

Worm::Worm(float x, float y, bool flip)
:Object(x,y)
{
	imageIndex = 0;
	hsp = 0.25 * ((flip)? -1: 1);
	vsp = 0;
}

void Worm::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.1, 4);

	//check if on ground
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+8, 16, 12 };
	mask.y += 1;
	bool onground = solidCollision(mask);
	mask.y -= 1;

	if (!onground)
	{
		//fall
		y += (vsp += grav);

		//land on ground
		mask.y = (int)y+8;
		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			vsp = 0;
			y = collide.y - 20;
		}
	}
	else
	{
		//turn on ledge
		mask.x += mask.w * sign(hsp);
		mask.y += 4;

		if (!solidCollision(mask))
			hsp *= -1;
	}

	//horizontal movement
	{
		x += hsp;

		//turn if it hit a wall
		mask = (Rectangle){ (int)x+2, (int)y+8, 16, 12 };
		mask.h -= 2;

		Rectangle collide;
		if (solidCollision(mask, &collide))
		{
			x = collide.x - 18 * sign(hsp);
			hsp *= -1;
		}
	}

	//collisions
	hero.hit(mask, 15);

	if (weaponList.collide(mask))
	{
		aud_PlaySound(sounds[sndBom01]);
		effectList.add(new Poof(x+10,y));
		dropCollectable(x,y);
		dead = true;
	}
}

void Worm::draw(float subFrame, float depth)
{
	int animation[] = { 1, 0, 2, 0 };
	int tile = 16 + animation[(((int)imageIndex) % 4)];

	if (hsp < 0)
		tile += 3;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	gfx_DrawTile(images[imgEne01], dx, dy+5, 20, 20, tile);
}