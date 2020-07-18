#include "pendulum.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../math.hpp"
#include "../game.hpp"

Pendulum::Pendulum(float x, float y, int offset)
:Object(x+10,y+20)
{
	ang[0] = 0;
	ang[1] = ang[0];
	rot = 180 + offset;

	layer = 1;
}

void Pendulum::update()
{
	Object::update();
	ang[1] = ang[0];

	rot = (rot + 2) % 360;
	ang[0] = 90 + (90 * lengthdir_x(1, rot));

	//collisions
	Circle mask = (Circle) { (int)(x+lengthdir_x(48,ang[0])), (int)(y+lengthdir_y(48,ang[0])), 12 };

	hero.hit(mask, 15);

	if (weaponList.collide(mask, this))
	{
		hitTimer = 15;
		aud_PlaySound(sounds[sndHit03]);
	}
}

void Pendulum::draw(float subFrame, float depth)
{
	float drawAng = lerp(ang[0], ang[1], subFrame);
	float dx, dy;

	int length[] = { 0, 8, 16, 24, 33, 48 };
	int tile[] = { 21, 21, 21, 21, 20, 19 };
	float z[] = { 0, 0, 0, 0, -1, -3 };

	for (int i = 0; i < 6; i++)
	{
		dx = x + lengthdir_x(length[i], drawAng);
		dy = y + lengthdir_y(length[i], drawAng);

		dx += z[i] * depth;

		gfx_DrawTile(images[imgChr32], dx-16, dy-16, 32, 32, tile[i]);
	}
}