#include "guruguru.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../hero.hpp"
#include "../game.hpp"
#include "../effects/poof.hpp"
#include "../objects/collectable.hpp"

GuruGuru::GuruGuru(float x, float y, int quarterTurns, bool counterClockwise)
:Object(x,y)
{
	hp = 2;
	direction = 90 * quarterTurns;
	timer = 0;
	clockwise = !counterClockwise;
	imageIndex = 0;
}

static int move_x(int len, int ang)
{
	if (ang < 0) ang += 360;
	if (ang >= 360) ang -= 360;

	if (ang == 0)
		return len;
	if (ang == 180)
		return -len;

	return 0;
}

static int move_y(int len, int ang)
{
	if (ang < 0) ang += 360;
	if (ang >= 360) ang -= 360;

	if (ang == 90)
		return len;
	if (ang == 270)
		return -len;

	return 0;
}

void GuruGuru::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	//animate
	imageIndex = fmod(imageIndex + 0.3, 4);

	//movement
	int speed = 1;
	x += move_x(speed, direction);
	y += move_y(speed, direction);

	//hit wall
	if (solidCollision(x+10+move_x(10, direction), y+10+move_y(10, direction)))
	{
		x = floor((x+10) / 20) * 20;
		y = floor((y+10) / 20) * 20;
		direction += ((clockwise)? 90: -90);
	}
	else if (timer <= 0)
	{
		//turn on edge
		int check = direction + ((clockwise)? 90: -90);
		Rectangle mask = (Rectangle){ (int)x+1+move_x(20,check), (int)y+1+move_y(20,check), 18, 18 };

		if (!solidCollision(mask))
		{
			x = floor((x+10) / 20) * 20;
			y = floor((y+10) / 20) * 20;
			direction = check;
			timer = 10;
		}
	}

	if (direction < 0) direction += 360;
	if (direction >= 360) direction -= 360;

	//collisions
	Circle mask = (Circle){ (int)x+10, (int)y+10, 8 };

	hero.hit(mask, 20);

	if (weaponList.collide(mask, this))
	{
		hp -= 1;
		hitTimer = 15;

		if (hp > 0)
		{
			aud_PlaySound(sounds[sndHit02]);
		}
		else
		{
			aud_PlaySound(sounds[sndBom01]);
			effectList.add(new Poof(x+10,y));
			dropCollectable(x,y);
			dead = true;
		}
	}
}

void GuruGuru::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 44, 60 };

	gfx_DrawTile(images[imgEne01], dx, dy, 20, 20, tile[!clockwise] + (int)imageIndex);
}