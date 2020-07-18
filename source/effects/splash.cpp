#include "splash.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"

#define grav (0.1)
#define zspd (-0.1)

void createSplash(float x, float y, bool lava)
{
	float hsp[] = { -0.75, -0.5, 0.75, 0.5 };
	float vsp;

	for (int i = 0; i < 4; i++)
	{
		vsp = -1.5 - (rand() % 6) / 2.f;
		effectList.add(new Splash(x,y,hsp[i],vsp,lava));
	}
}

Splash::Splash(float x, float y, float hsp, float vsp, bool lava)
:Object(x,y)
{
	this->lava = lava;
	z = 0;
	this->hsp = hsp;
	this->vsp = vsp;
	imageIndex = 0;
	layer = 2;
}

void Splash::update()
{
	Object::update();

	x += hsp;
	y += (vsp += grav);
	z += zspd;

	if ((imageIndex += 0.1) >= 6)
		dead = true;
}

void Splash::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile[] = { 5, 90 };

	gfx_DrawTile(images[imgChr20], dx-10+z*depth, dy-10, 20, 20, tile[lava] + (int)imageIndex);
}