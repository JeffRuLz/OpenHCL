#pragma once

#include "collision.hpp"

enum 
{
	TILE_NONE   = 0,
	TILE_SOLID  = (1 << 0),
	TILE_LADDER = (1 << 1),
	TILE_WATER  = (1 << 2),
	TILE_LAVA   = (1 << 3),
	TILE_SPIKE  = (1 << 4),
	TILE_ANY	= 0xFF
};

typedef struct
{
	unsigned char data[22*18];
} Map;

extern Map map[2];

#define TILE_SIZE 20

void loadMap(int num, bool background);

bool mapCollision(int x, int y, unsigned char flags = TILE_ANY, Rectangle* out = nullptr);
bool mapCollision(Rectangle r, unsigned char flags = TILE_ANY, Rectangle* out = nullptr);

bool solidCollision(int x, int y, Rectangle* out = nullptr);
bool solidCollision(Rectangle r, Rectangle* out = nullptr);