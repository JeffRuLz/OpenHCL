#include "map.hpp"
#include "system.hpp"
#include "game.hpp"
#include <stdio.h>

Map map[2];

//collision attributes for each tile
//reference the tileset graphics
//00000000 - 0x00 - Nothing
//00000001 - 0x01 - Solid
//00000010 - 0x02 - Ladder
//00000100 - 0x04 -	Water
//00001000 - 0x08 - Lava
//00010000 - 0x16 - Spike
static unsigned char tileFlags[16*12] =
{
	0x00, 0x00, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x08, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

void loadMap(int num, bool background)
{
	char fpath[256];
	sprintf(fpath, "%smap/%03d%s.map", SYS_DATAPATH, num, (background)? "a": "");

	FILE* f = fopen(fpath, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("loadMap() could not open %s\n", fpath);
		#endif
	}
	else
	{
		//skip header
		fseek(f, 24, SEEK_SET);

		unsigned short byte = 0;

		for (int i = 0; i < 22*18; i++)
		{
			fread(&byte, 1, 2, f);
			map[!background].data[i] = (unsigned char)sys_ByteSwap16(byte);
		}
	}

	fclose(f);
}

bool mapCollision(int x, int y, unsigned char flags, Rectangle* out)
{
	x = x / TILE_SIZE + 3;
	y = y / TILE_SIZE + 3;

	if (x < 0 || x > 22 || y < 0 || y > 18)
	{}
	else
	{
		if (tileFlags[map[1].data[x + y * 22]] & flags)
		{
			if (out != nullptr)
			{
				out->x = (x - 3) * TILE_SIZE;
				out->y = (y - 3) * TILE_SIZE;
				out->w = TILE_SIZE;
				out->h = TILE_SIZE;
			}

			return true;
		}
	}

	return false;
}

bool mapCollision(Rectangle r, unsigned char flags, Rectangle* out)
{
	for (int y = (r.y+60) / (float)TILE_SIZE; y < (r.y+60+r.h) / (float)TILE_SIZE; y++)
	{
		for (int x = (r.x+60) / (float)TILE_SIZE; x < (r.x+60+r.w) / (float)TILE_SIZE; x++)
		{
			if (mapCollision(x*TILE_SIZE-60,y*TILE_SIZE-60,flags,out))
				return true;
		}
	}

	return false;
}

bool solidCollision(int x, int y, Rectangle* out)
{
	//map collision
	if (mapCollision(x, y, TILE_SOLID, out))
		return true;

	//object collision
	Object* ptr = platformList.next(nullptr);
	
	while (ptr != nullptr)
	{
		if (ptr->isSolid())
		{
			if (collision(x, y, (Rectangle){ (int)ptr->getX(), (int)ptr->getY(), 20, 20 }))
			{
				if (out != nullptr)
				{
					out->x = ptr->getX();
					out->y = ptr->getY();
					out->w = 20;
					out->h = 20;
				}

				return true;
			}
		}

		ptr = platformList.next(ptr);
	}

	return false;
}

bool solidCollision(Rectangle r, Rectangle* out)
{
	//map collision
	if (mapCollision(r, TILE_SOLID, out))
		return true;

	//object collision
	Object* ptr = platformList.next(nullptr);
	
	while (ptr != nullptr)
	{
		if (ptr->isSolid())
		{
			if (collision(r, (Rectangle){ (int)ptr->getX(), (int)ptr->getY(), 20, 20 }))
			{
				if (out != nullptr)
				{
					out->x = ptr->getX();
					out->y = ptr->getY();
					out->w = 20;
					out->h = 20;
				}

				return true;
			}
		}

		ptr = platformList.next(ptr);
	}

	return false;
}