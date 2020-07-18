#include "chargeorb.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"

static int lastAng = 0;

ChargeOrb::ChargeOrb()
:Object(0,0)
{
	timer = 30;
	imageIndex = 0;
	do {
		ang = rand() % 360;
	} while (abs(ang-lastAng) < 45);
	lastAng = ang;	

	x = prevx = hero.getX() + lengthdir_x(timer, ang);
	y = prevy = hero.getY() + lengthdir_y(timer, ang);

	layer = 0;
}

void ChargeOrb::update()
{
	Object::update();

	if (timer > 0)
		timer -= 1;

	x = hero.getX() + lengthdir_x(timer, ang);
	y = hero.getY() + lengthdir_y(timer, ang);

	if ((imageIndex += 0.15) >= 6)
		dead = true;
}

void ChargeOrb::draw(float subFrame, float depth)
{
	if (timer % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgChr20], dx-10, dy-10, 20, 20, 80 + (int)imageIndex);
}