#include "sword.hpp"
#include "../hero.hpp"
#include "../assets.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/tink.hpp"

Sword::Sword(float x, float y)
:Object(x,y)
{
	imageIndex = 0;
	mask = (Rectangle){ 0, 0, 10, 10 };
}

void Sword::update()
{
	Object::update();

	imageIndex = hero.getImageIndex();
	setDirection(hero.getDirection());

	x = hero.getX();
	y = hero.getY();

	//update mask
	if (imageIndex < 2)
	{
		mask = (Rectangle) {
			(int)x - 10 + (direction * 14) + 2,
			(int)y - 13,
			16, 19
		};
	}
	else if (imageIndex < 3)
	{
		mask = (Rectangle) {
			(int)x - 10 + (direction * 13) + 4,
			(int)y - 1,
			12, 10
		};
	}
	else if (imageIndex < 4)
	{
		mask = (Rectangle) {
			(int)x - 10 + (direction * 13) + 4,
			(int)y + 5,
			12, 3
		};	
	}

	//finish slash
	HeroState hs = hero.getState();

	if ((hs != HERO_STATE_SLASH && hs != HERO_STATE_CHARGE) || imageIndex >= 4)
	{
		dead = true;
	}
}

void Sword::draw(float subFrame, float depth)
{
	if (hero.getInvincibleTimer() % 2)
		return;

	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	int tile = 96 + (int)imageIndex % 4;
	int addx = 0;
	int addy = -10;

	if (imageIndex < 1)
	{
		addx = -4;
		addy = -18;
	}
	else if (imageIndex < 2)
	{
		addx = 12;
		addy = -14;
	}
	else
	{
		addx = 13;
		addy = -3;
	}

	if (direction == -1)
	{
		addx *= -1;
		tile += 4;
	}

	//gfx_DrawRect(mask.x, mask.y, mask.x+mask.w, mask.y+mask.h, (Color){ 0x00, 0xff, 0x00 });
	gfx_DrawTile(images[imgMychr], dx - 10 + addx, dy + addy, 20, 20, tile);
}

static void _addEffect(float x1, float y1, float x2, float y2)
{
	float x = ((x1 + x2) / 2) - 10 + (rand() % 20);
	float y = ((y1 + y2) / 2) - 10 + (rand() % 20);
	effectList.add(new Tink(x,y));
}

bool Sword::collide(Rectangle r, Object* obj)
{
	if (imageIndex < 1)
		return false;

	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}

	if (collision(r, mask))
	{
		_addEffect(mask.x+mask.w/2, mask.y+mask.h/2, r.x+r.w/2, r.y+r.h/2);
		return true;
	}

	return false;
}

bool Sword::collide(Circle c, Object* obj)
{
	if (imageIndex < 1)
		return false;

	if (obj)
	{
		if (obj->getHitTimer() > 0)
			return false;
	}
	
	if (collision(c, mask))
	{
		_addEffect(mask.x+mask.w/2, mask.y+mask.h/2, c.x, c.y);
		return true;
	}

	return false;
}