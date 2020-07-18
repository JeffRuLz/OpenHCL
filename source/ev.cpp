#include "ev.hpp"
#include "system.hpp"
#include "game.hpp"
#include <stdio.h>
#include <string.h>

#include "enemies/bat.hpp"
#include "enemies/batboss.hpp"
#include "enemies/bee.hpp"
#include "enemies/boar.hpp"
#include "enemies/crushtrap.hpp"
#include "enemies/devil.hpp"
#include "enemies/dodo.hpp"
#include "enemies/garm.hpp"
#include "enemies/gas.hpp"
#include "enemies/ghost.hpp"
#include "enemies/golem.hpp"
#include "enemies/golgia.hpp"
#include "enemies/guruguru.hpp"
#include "enemies/gyra.hpp"
#include "enemies/hydra.hpp"
#include "enemies/jelly.hpp"
#include "enemies/jellyfish.hpp"
#include "enemies/kappa.hpp"
#include "enemies/knight.hpp"
#include "enemies/labyrush.hpp"
#include "enemies/lolidra.hpp"
#include "enemies/piranha.hpp"
#include "enemies/pendulum.hpp"
#include "enemies/skeleton.hpp"
#include "enemies/statue.hpp"
#include "enemies/tongue.hpp"
#include "enemies/wizard.hpp"
#include "enemies/worm.hpp"
#include "enemies/zombie.hpp"

#include "objects/block.hpp"
#include "objects/bossmusiccontroller.hpp"
#include "objects/chest.hpp"
#include "objects/crown.hpp"
#include "objects/door.hpp"
#include "objects/floorbutton.hpp"
#include "objects/gate.hpp"
#include "objects/generator.hpp"
#include "objects/ladder.hpp"
#include "objects/lightswitch.hpp"
#include "objects/platform.hpp"
#include "objects/savepoint.hpp"
#include "objects/secretcontroller.hpp"
#include "objects/statueblock.hpp"

static bool _readLine(FILE* f, char* out)
{
	char c = ' ';
	bool firstChar = true;

	while (c != '\0')
	{
		if (fread(&c, 1, 1, f) != 1)
			return false;

		if (c == EOF)
			c = '\n';

		if (c == '\n')
		{
			if (firstChar)
				return false;

			c = '\0';
		}

		*(out++) = c;
		firstChar = false;
	}

	return true;
}

static char* _chrrep(char* str, char find, char replace)
{
	char* ptr = str;

	while (*ptr != '\0')
	{
		if (*ptr == find)
			*ptr = replace;

		ptr += 1;
	}

	return str;
}

static int _parseNumber(char* ptr, int part = 0)
{
	if (ptr == nullptr)
		return 0;

	int n = 0;
	int sign = 1;

	while (part > 0)
	{
		if (*ptr == ',' || *ptr == '\0')
			return 0;

		if (*ptr == '-' && *(ptr-1) != '-')
			part -= 1;

		ptr += 1;
	}

	if (*ptr == '-' && *(ptr-1) == '-')
	{
		sign = -1;
		ptr += 1;
	}

	while (*ptr >= '0' && *ptr <= '9')
	{
		n *= 10;
		n += *ptr - '0';
		ptr += 1;
	}

	return n * sign;
}

static void _parseM(char* str, int &x, int &y)
{
	char* ptr = strrchr(str, 'M');

	if (ptr != nullptr)
	{
		ptr += 1;

		int sign = 1;
		int val = 0;
		bool error = false;

		for (int i = 0; i < 4; i++)
		{
			if (*ptr == ',')
			{
				error = true;
				break;
			}

			if (!error)
			{
				if (i == 0 && *ptr == '-')
					sign = -1;
				else
				{
					if (*ptr < '0' || *ptr > '9')
					{
						error = true;
					}
					else
					{
						val *= 10;
						val += *ptr - '0';
					}
				}
			}

			ptr += 1;
		}

		if (!error)
			x += (val * sign) / 2;

		//
		sign = 1;
		val = 0;
		error = false;

		for (int i = 0; i < 4; i++)
		{
			if (*ptr == ',')
			{
				error = true;
				break;
			}

			if (!error)
			{
				if (i == 0 && *ptr == '-')
					sign = -1;
				else
				{
					if (*ptr < '0' || *ptr > '9')
					{
						error = true;
					}
					else
					{
						val *= 10;
						val += *ptr - '0';
					}
				}
			}

			ptr += 1;
		}

		if (!error)
			y += (val * sign) / 2;
	}
}

bool loadEv(int index)
{
	char str[128];
	sprintf(str, "%sdata/EV/EV%03d", SYS_DATAPATH, index);

	FILE* f = fopen(str, "rb");

	if (!f)
	{
		#ifdef _DEBUG
		printf("loadEv() - Could not open %s\n", str);
		#endif
		return false;
	}
	else
	{
		printf("\n");

		while (_readLine(f, str))
		{
			//char* ptr = str;

			char* section[7] = { nullptr };

			for (int i = 0; i < 7; i++)
			{
				if (i == 0)
					section[0] = str;
				else
					section[i] = strchr(section[i-1], ',') + 1;
			}

			_chrrep(str, ',', '\0');
			_chrrep(str, '\n', '\0');

			for (int i = 0; i < 7; i++)
				printf("%s ", section[i]);
			printf("\n");

			//int unknown 	= _parseNumber(section[0]);
			int spawnFlag 	= _parseNumber(section[1]);
			int despawnFlag = _parseNumber(section[2]);
			int x 			= _parseNumber(section[3]) * 20;
			int y 			= _parseNumber(section[4]) * 20;
			int obj 		= _parseNumber(section[5]);
			int subtype 	= _parseNumber(section[5],2);
			int killFlag 	= _parseNumber(section[6]);
			int numOfFlags  = _parseNumber(section[6],1);
			int flags[20] = { -1 };

			for (int i = 0; i < 20; i++)
			{
				flags[i] = _parseNumber(section[6],2+i);
				if (flags[i] > 10000)
					flags[i] -= 10000;
			}

			_parseM(section[5], x, y);

			x -= 20*3;
			y -= 20*3;

			Object* o = nullptr;

			switch (obj)
			{
				case 0:
				{
					switch (subtype)
					{
						case 3: o = objectList.add(new BossMusicController(_parseNumber(section[6],0))); break;
						case 4: o = objectList.add(new SecretController(killFlag, numOfFlags, flags)); break;
						case 5: o = objectList.add(new Ladder((_parseNumber(section[5],3)-3)*20, (_parseNumber(section[5],4)-2)*20, _parseNumber(section[5],5))); break;
					}
				}
				break;
				case 41: o = enemyList.add(new Jelly(x,y,subtype,_parseNumber(section[5],3))); break;
				case 42: o = enemyList.add(new Bat(x,y,subtype)); break;
				case 43: o = enemyList.add(new Dodo(x,y,_parseNumber(section[6],0))); break;
				case 44: o = enemyList.add(new Statue(x,y,subtype,_parseNumber(section[5],3),_parseNumber(section[5],4),_parseNumber(section[5],5))); break;
				case 45: o = platformList.add(new Platform(x,y,subtype,_parseNumber(section[5],3),_parseNumber(section[5],4), _parseNumber(section[5],5))); break;
				case 46:
				{
					switch (subtype)
					{
						case 0: o = enemyList.add(new Worm(x,y,_parseNumber(section[5],3))); break;
						case 1: o = enemyList.add(new Golem(x,y,_parseNumber(section[5],3))); break;
					}
				} 
				break;
				case 47: o = enemyList.add(new Boar(x,y)); break;
				case 48: o = enemyList.add(new Ghost(x,y)); break;
				case 49: o = enemyList.add(new Zombie(x,y,subtype)); break;
				case 50: o = enemyList.add(new Knight(x,y,subtype)); break;
				case 51: o = enemyList.add(new Skeleton(x,y,_parseNumber(section[5],3))); break;
				case 52: o = enemyList.add(new Gyra(x,y,_parseNumber(section[6],0))); break;
				case 53: o = enemyList.add(new Gas(x,y)); break;
				case 54: o = enemyList.add(new CrushTrap(x,y,subtype,0,_parseNumber(section[5],3),_parseNumber(section[5],4))); break;
				case 55: o = enemyList.add(new CrushTrap(x,y,2,_parseNumber(section[5],3),60,_parseNumber(section[5],4))); break;
				case 56: o = enemyList.add(new Labyrush(x,y,_parseNumber(section[5],3))); break;
				case 57: o = enemyList.add(new Kappa(x,y,subtype,_parseNumber(section[5],3),_parseNumber(section[5],4),_parseNumber(section[5],5))); break;
				case 58: o = enemyList.add(new Tongue(x,y,_parseNumber(section[5],4))); break;
				case 59: o = enemyList.add(new Piranha(x,y,_parseNumber(section[5],4))); break;
				case 60: o = enemyList.add(new GuruGuru(x,y,_parseNumber(section[5],3),_parseNumber(section[5],4))); break;
				case 61: o = enemyList.add(new BatBoss(x,y,_parseNumber(section[6],0))); break;
				case 62:
				{
					switch (subtype)
					{
						case 0: o = enemyList.add(new Jellyfish(x,y,_parseNumber(section[5],3))); break;
						case 1: o = enemyList.add(new Bee(x,y,_parseNumber(section[5],4))); break;
						case 2: o = enemyList.add(new Pendulum(x,y,_parseNumber(section[5],3))); break;
						case 4: o = enemyList.add(new Wizard(x,y,_parseNumber(section[6],0))); break;
					}
				}
				break;
				case 63: o = enemyList.add(new Hydra(x,y,_parseNumber(section[6],0))); break;
				case 65: o = enemyList.add(new Devil(x,y,_parseNumber(section[6],0))); break;
				case 66: o = enemyList.add(new Lolidra(x,y,_parseNumber(section[6],0))); break;
				case 67: o = enemyList.add(new Golgia(x,y,_parseNumber(section[6],0))); break;
				case 68: o = enemyList.add(new Garm(x,y,_parseNumber(section[6],0))); break;
				case 121:
				{
					switch (subtype)
					{
						case 2:  o = objectList.add(new Gate(x,y,_parseNumber(section[5],3),_parseNumber(section[5],4))); break;
						default: o = platformList.add(new Block(x,y,subtype)); break;
					}
				}
				break; 
				case 122: o = objectList.add(new Chest(x,y, _parseNumber(section[5],2), _parseNumber(section[5],3))); break;
				case 123: o = objectList.add(new Door(x,y,_parseNumber(section[5],3),_parseNumber(section[5],4), _parseNumber(section[6],0), _parseNumber(section[6],1), _parseNumber(section[6],2), _parseNumber(section[6],3))); break;
				case 124: o = objectList.add(new SavePoint(x,y)); break;
				case 125: o = objectList.add(new FloorButton(x,y,_parseNumber(section[5],3))); break;
				case 126:
				{
					switch (subtype)
					{
						case 2: o = platformList.add(new ElectricGate(x,y)); break;
						case 3: o = objectList.add(new Generator(x,y,_parseNumber(section[6],0))); break;
						default: o = objectList.add(new StatueBlock(x,y,subtype,_parseNumber(section[5],3))); break;
					}
				}
				break;
				case 127: o = objectList.add(new LightSwitch(x,y,_parseNumber(section[5],3),_parseNumber(section[5],4))); break;
				case 128: o = objectList.add(new Crown(x,y)); break;
			}

			if (o != nullptr)
			{
				o->setSpawnFlag(spawnFlag);
				o->setDespawnFlag(despawnFlag);
				o->setKillFlag(killFlag);

				if (!o->isDead())
					o->create();
			}
		}
	}

	fclose(f);
	return true;
}