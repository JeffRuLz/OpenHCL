#include "generator.hpp"
#include "../assets.hpp"
#include "../game.hpp"
#include "../hero.hpp"
#include "../math.hpp"
#include "../effects/brokenrock.hpp"

Generator::Generator(float x, float y, int flag)
:Object(x,y)
{
	hp = 3;
	this->flag = flag;
	imageIndex = 0;
}

void Generator::update()
{
	Object::update();

	imageIndex = fmod(imageIndex + 0.3, 2);

	Rectangle mask = (Rectangle){ (int)x+4, (int)y, 12, 20 };

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
			aud_PlaySound(sounds[sndBom02]);
			rockSmashEffect(x+10, y+10);
			dead = true;
		}
	}
}

void Generator::draw(float subFrame, float depth)
{
	if (hitTimer % 2)
		return;

	gfx_DrawTile(images[imgChr20], x, y, 20, 20, 242 + (int)imageIndex);
}


ElectricGate::ElectricGate(float x, float y)
:Object(x,y+8)
{
	imageIndex = 0;
}

void ElectricGate::update()
{
	imageIndex = fmod(imageIndex + 0.3, 4);

	if (hero.getState() == HERO_STATE_LADDER)
	{
		Rectangle mask = (Rectangle){ (int)x, (int)y, 20, 4 };

		if (collision(mask, hero.getMask()))
		{
			hero.setY(mask.y-10);
			hero.setVsp(0);
		}
	}

	if (gameGetFlag(despawnFlag))
		dead = true;
}

void ElectricGate::draw(float subFrame, float depth)
{
	gfx_DrawTile(images[imgChr20], x, y-8, 20, 20, 226 + (int)imageIndex);
}