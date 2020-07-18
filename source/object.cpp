#include "object.hpp"
#include "game.hpp"
#include <stdio.h>

void Object::setDespawnFlag(int f)
{
	if (f != 0)
	{
		despawnFlag = f;

		if (gameGetFlag(despawnFlag))
			dead = true;
	}
};

ObjectList::ObjectList()
{
	size = 0;

	for (int i = 0; i < 100; i++)
		list[i] = nullptr;
}

ObjectList::~ObjectList()
{
	clear();
}

void ObjectList::update()
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (list[i]->isDead() == false)
			{
				int f = list[i]->getSpawnFlag();
				if (f == 0 || gameGetFlag(f))
					list[i]->update();
			}
		}
	}

	//free dead objects
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (list[i]->isDead() || gameGetFlag(list[i]->getDespawnFlag()))
			{
				#ifdef _DEBUG
				int f = list[i]->getKillFlag();
				if (f != 0)
					printf("%d\n", f);
				#endif
				gameSetFlag(list[i]->getKillFlag(), true);

				delete list[i];
				list[i] = nullptr;
			}
		}
	}
}

void ObjectList::draw(float subFrame, float depth, int layer)
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (list[i]->isDead() == false)
			{
				int f = list[i]->getSpawnFlag();
				if (f == 0 || gameGetFlag(f))
				{
					if (layer == -1 || layer == list[i]->getLayer())
						list[i]->draw(subFrame, depth);
				}
			}
		}
	}
}

Object* ObjectList::add(Object* o)
{
	for (int i = 0; i < 100; i++)
	{
		if (list[i] == nullptr)
		{
			if (i+1 > size)
				size = i+1;

			list[i] = o;
			return o;
		}
	}

	//add failed
	delete o;	
	return nullptr;
}

void ObjectList::clear()
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			delete list[i];
			list[i] = nullptr;
		}
	}

	size = 0;
}

int ObjectList::count()
{
	int n = 0;

	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (list[i]->isDead() == false)
				n += 1;
		}
	}

	return n;
}

Object* ObjectList::next(Object* o)
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (o == nullptr)
				return list[i];
			else
			{
				if (list[i] == o)
					o = nullptr;
			}
		}
	}

	return nullptr;
}

Object* ObjectList::collide(Rectangle r, Object* obj)
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (!list[i]->isDead() && list[i]->collide(r, obj))
				return list[i];
		}
	}

	return nullptr;
}

Object* ObjectList::collide(Circle c, Object* obj)
{
	for (int i = 0; i < size; i++)
	{
		if (list[i] != nullptr)
		{
			if (!list[i]->isDead() && list[i]->collide(c, obj))
				return list[i];
		}
	}

	return nullptr;
}