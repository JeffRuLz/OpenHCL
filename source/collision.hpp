#pragma once

typedef struct
{
	int x, y;
	int w, h;
} Rectangle;

typedef struct
{
	int x, y;
	int r;
} Circle;

bool collision(Rectangle r1, Rectangle r2);
bool collision(Circle c1, Circle c2);
bool collision(Rectangle r, Circle c);
bool collision(Circle c, Rectangle r);

bool collision(int x, int y, Rectangle r);
bool collision(int x, int y, Circle c);