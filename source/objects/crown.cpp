#include "crown.hpp"
#include "../assets.hpp"
#include "../hero.hpp"
#include "../math.hpp"
#include "../game.hpp"
#include "../effects/sparkle.hpp"

Crown::Crown(float x, float y)
:Object(x+0.5,y)
{
	ystart = this->y;
	rot = 0;
	timer = 0;
	imageIndex = 0;
}

void Crown::update()
{
	Object::update();

	//animate
	imageIndex = fmod(imageIndex += 0.3, 2);
	rot = (rot + 3) % 360;
	y = ystart + lengthdir_y(5, rot);

	//sparkle
	if ((timer -= 1) <= 0)
	{
		timer = 10;
		float ex = x + (rand() % 20);
		float ey = y + (rand() % 20);
		effectList.add(new Sparkle(ex,ey));
	}

	//collision
	Rectangle mask = (Rectangle){ (int)x+2, (int)y+4, 15, 11 };

	if (collision(hero.getX(), hero.getY(), mask))
	{
		aud_PlaySound(sounds[sndGet02]);
		effectList.clear();
		gameSetState(GAME_STATE_RESULTS);
		dead = true;
	}
}

void Crown::draw(float subFrame, float depth)
{
	float dy = lerp(y, prevy, subFrame);
	gfx_DrawTile(images[imgMychr], x, dy, 20, 20, 80 + (int)imageIndex);
}