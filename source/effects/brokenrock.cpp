#include "brokenrock.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"

#define grav (0.1)
#define zpd (-0.1)

void rockSmashEffect(float x, float y)
{
	float hsp[] = { -0.75, -0.5, 0.75, 0.5 };
	float vsp;

	for (int i = 0; i < 4; i++)
	{
		vsp = -1.5 - (rand() % 6) / 2.f;
		effectList.add(new BrokenRock(x,y+10,hsp[i],vsp));
	}
}

BrokenRock::BrokenRock(float x, float y, float hsp, float vsp)
:Object(x,y)
{
	this->hsp = hsp;
	this->vsp = vsp;
	z = 0;
	imageIndex = 0;
	timer = 0;
	big = rand() % 2;

	layer = 2;
}

void BrokenRock::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex + 0.2, 4);

	//movement
	x += hsp;
	y += (vsp += grav);
	z += zpd;

	if (timer++ >= 60)
		dead = true;
}

void BrokenRock::draw(float subFrame, float depth)
{
	if (timer > 30 && timer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	dx += z * depth;

	int tile[] = { 176, 16 };
	int t = tile[big] + (int)imageIndex + (4 * (hsp < 0));

	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, t);
}