#include "collision.hpp"
#include "math.hpp"

bool collision(Rectangle r1, Rectangle r2)
{
	return
		!(r1.x > r2.x+r2.w-1 || r1.y > r2.y+r2.h-1 ||
		  r1.x+r1.w-1 < r2.x || r1.y+r1.h-1 < r2.y);
}

bool collision(Circle c1, Circle c2)
{
	int x = c2.x - c1.x;
	int y = c2.y - c1.y;
	int dis = c1.r + c2.r;

	return ( (x*x) + (y*y) < dis*dis );
}

bool collision(Rectangle r, Circle c)
{
	if (collision(c.x, c.y, r))
		return true;

	else
	{
		int x = clamp(c.x, r.x, r.x+r.w);
		int y = clamp(c.y, r.y, r.y+r.h);
		return collision(x, y, c);
	}
}

bool collision(Circle c, Rectangle r)
{
	return collision(r, c);
}

bool collision(int x, int y, Rectangle r)
{
	return !(x < r.x || y < r.y || x > r.x+r.w || y > r.y+r.h);
}

bool collision(int x, int y, Circle c)
{
	return (distance(x,y,c.x,c.y) <= c.r);
}