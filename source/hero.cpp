#include "hero.hpp"
#include "assets.hpp"
#include "input.hpp"
#include "math.hpp"
#include "map.hpp"
#include "game.hpp"
#include "text.hpp"

#include "effects/airbubble.hpp"
#include "effects/brokenrock.hpp"
#include "effects/chargeorb.hpp"
#include "effects/dust.hpp"
#include "effects/pebble.hpp"
#include "effects/poisonbubble.hpp"
#include "effects/poof.hpp"
#include "effects/shock.hpp"
#include "effects/splash.hpp"

#include "weapons/sword.hpp"
#include "weapons/arrow.hpp"
#include "weapons/axe.hpp"
#include "weapons/boomerang.hpp"
#include "weapons/fireball.hpp"
#include "weapons/bomb.hpp"

static const float walkSpeed = 1.5f;
static const float jumpSpeed = 3.9f;
static const float gravity = 0.15f;
static const float climbSpeed[2] = { 0.5f, 1.f };
static const int maxProjectiles[2] = { 2, 3 };

Hero::Hero(float x, float y)
:Object(x,y)
{
	state = HERO_STATE_NORMAL;

	hsp = 0;
	vsp = 0;

	imageIndex = 0;

	hp = maxhp = 128;
	maxammo = 99;
	ammo = 0;

	weapon = 0;
	stoneState = 0;

	chargeTimer = 0;
	deathTimer = 0;
	drownTimer = 0;
	hitTimer = 0;
	invincibleTimer = 0;
	poisonTimer = 0;
	shieldTimer = 0;
	stoneTimer = 0;
	stunTimer = 0;
	quakeCounter = 0;	

	heldUp = false;
	onGround = false;
	inWater = false;
	canCharge = false;
	canDoubleJump = false;
}

void Hero::update()
{
	Object::update();

	heldUp = inp_ButtonHeld(BTN_UP);

	if (stunTimer > 0)
	{
		stunTimer -= 1;

		if (stunTimer <= 0)
			aud_PlaySound(sounds[sndPower01]);

		if (((300 - stunTimer) % 32) / 4 == 0)
			aud_PlaySound(sounds[sndHit05]);
	}

	//poison bubble effect
	if (poisonTimer > 0)
	{
		poisonTimer -= 1;

		if (poisonTimer % 20 == 0)
		{
			if (hp > 1)
				hp -= 1;
			else
				hp = 0;

			effectList.add(new PoisonBubble(x-10-5+(rand()%10), y-10));
			aud_PlaySound(sounds[sndPi02]);
		}
	}

	//Non-controllable states
	if (state == HERO_STATE_DOOR)
	{
		//remove status conditions
		stunTimer = 0;
		poisonTimer = 0;
		inWater = false;

		//animate
		//end
		if ((imageIndex += 0.2) >= 10)
		{
			state = HERO_STATE_NORMAL;
			gameSetState(GAME_STATE_TRANSITION);
		}

		return;
	}

	else if (state == HERO_STATE_DEATH)
	{
		stunTimer = 0;
		poisonTimer = 0;
		//drownTimer = 0;
		invincibleTimer = 0;

		deathTimer += 1;

		//animate
		imageIndex = fmod(imageIndex + 0.3, 4);

		//poof
		if (deathTimer == 90)
		{
			aud_PlaySound(sounds[sndBom01]);
			effectList.add(new Poof(x,y-10));
		}			

		//play music
		if (deathTimer == 150)
			aud_PlayMusic(music[bgmGameOver], false);

		//end game over screen early
		if (deathTimer > 150 && (inp_ButtonPressed(BTN_START) || inp_ButtonPressed(BTN_SELECT)))
			deathTimer = 630;

		//reset game
		if (deathTimer >= 630)
		{
			aud_StopMusic();
			#ifndef _DEBUG
			sys_DeleteSaveFile(SYS_TEMPSAVEPATH);
			#endif
			gameSetState(GAME_STATE_TITLESCREEN);
		}

		return;
	}

	else if (state == HERO_STATE_ITEM_GET)
	{
		imageIndex += 0.3;

		if (imageIndex >= 3 && imageIndex < 4)
		{
			imageIndex = 3;

			//end text box
			if (inp_ButtonPressed(BTN_JUMP) ||
				inp_ButtonPressed(BTN_ATTACK) ||
				inp_ButtonPressed(BTN_WEAPON) ||
				inp_ButtonPressed(BTN_ACCEPT) ||
				inp_ButtonPressed(BTN_DECLINE))
				imageIndex = 4;
		}

		if (imageIndex >= 7)
			state = HERO_STATE_NORMAL;

		return;
	}

	//check if in water
	if (inWater != mapCollision(x,y,TILE_WATER))
	{
		aud_PlaySound(sounds[sndWater01]);
		createSplash(x,y);
		inWater = !inWater;
		drownTimer = 60;
	}

	//drowning
	if (inWater)
	{
		drownTimer -= 1;

		if (drownTimer <= 0)
		{
			drownTimer = 60;
			if (!gameHasItem(ITEM_GILL))
			{
				if (hp > 4)
					hp -= 4;
				else
					hp = 0;
			}

			aud_PlaySound(sounds[sndPi06]);
			effectList.add(new AirBubble(x,y));
		}
	}

	//
	bool canGrav = true;
	float grav = gravity;

	changeWeapon();

	if (invincibleTimer > 0)
		invincibleTimer -= 1;

	//Non-controllable states, but can still move
	if (state == HERO_STATE_CHARGE)
	{
		canGrav = false;
		shieldTimer = 10;
		vsp = 0;

		//charge start (rear back)
		if (chargeTimer == 0)
		{
			imageIndex = 0;
			hsp = -1 * direction;
		}

		//friction
		float fric = 0.15;
		if (hsp < 0)
		{
			if ((hsp += fric) > 0)
				hsp = 0;
		}
		else if (hsp > 0)
		{
			if ((hsp -= fric) < 0)
				hsp = 0;
		}

		chargeTimer += 1;

		//forward charge start
		if (chargeTimer == 15)
		{
			invincibleTimer = 35;
			hsp = 3.5 * direction;
			aud_PlaySound(sounds[sndShot01]);
		}

		//animation
		if (chargeTimer > 15) imageIndex = 1;
		if (chargeTimer > 19) imageIndex = 2;
		if (chargeTimer > 21) imageIndex = 3;
		if (chargeTimer == 59) imageIndex = 4;

		//stop
		if (chargeTimer == 30)
			hsp = 0;

		//end state
		if (chargeTimer >= 60)
		{
			chargeTimer = 0;
			state = HERO_STATE_NORMAL;
		}
 	}

	else if (state == HERO_STATE_HIT)
	{
		chargeTimer = 0;
		grav = gravity - 0.025;

		//animate
		imageIndex = fmod(imageIndex + 0.3, 2);

		if (onGround)
			hsp = 0;

		//end hit state
		hitTimer -= 1;
		if (onGround && !vsp && hitTimer <= 0)
		{
			state = HERO_STATE_NORMAL;
			invincibleTimer = 60;
		}
	}

	else if (state == HERO_STATE_STONE)
	{
		grav = gravity - 0.025;

		switch (stoneState)
		{
			//in air
			case 0:
			{
				imageIndex = 0;

				if (onGround)
				{
					stoneState = 1;
					effectList.add(new Pebble(x,y));
					effectList.add(new Pebble(x,y));
				}
			}
			break;

			//frozen
			case 1:
			{
				hsp = 0;
				imageIndex = fmod(imageIndex + 0.15, 2);

				if (inp_ButtonPressed(BTN_JUMP) || inp_ButtonPressed(BTN_ATTACK) || inp_ButtonPressed(BTN_WEAPON))
				{
					stoneTimer -= 30;
					effectList.add(new Pebble(x,y));
				}

				if (stoneTimer > 0)
					stoneTimer -= 1;
				else
				{
					stoneState = 2;
					imageIndex = 0;

					aud_PlaySound(sounds[sndBom02]);
					rockSmashEffect(x,y+10);
					effectList.add(new Shock(x,y));
				}
			}
			break;

			//break free
			case 2:
			{
				hsp = 0;
				imageIndex += 0.3;

				if (imageIndex >= 17)
				{
					stoneState = 0;
					state = HERO_STATE_NORMAL;
				}
			}
			break;
		}
	}

	else if (state == HERO_STATE_QUAKE)
	{
		grav = gravity - 0.025;
		hsp = 0;

		if (onGround)
		{
			aud_PlaySound(sounds[sndPi02]);
			quakeCounter += 1;

			if (quakeCounter <= 3)
			{
				vsp = (-1.f / quakeCounter) - grav;
				onGround = false;
			}
			else
			{
				state = HERO_STATE_NORMAL;
				vsp = 0;
			}
		}
	}

	//Controllable states
	else
	{
		bool canWalk = true;

		if (state == HERO_STATE_NORMAL)
		{
			if (shieldTimer > 0)
				shieldTimer -= 1;

			//Change direction
			if (inp_ButtonHeld(BTN_RIGHT)) direction = 1;
			if (inp_ButtonHeld(BTN_LEFT)) direction = -1;

			//Jump
			if (inp_ButtonPressed(BTN_JUMP))
			{
				bool doJump = false;

				if (onGround)
				{
					doJump = true;
				}
				else if (canDoubleJump && gameHasItem(ITEM_BLUE_BOOTS))
				{
					doJump = true;
					canDoubleJump = false;
				}

				if (doJump)
				{
					vsp = -jumpSpeed;
					onGround = false;
					aud_PlaySound(sounds[sndJump01]);
				}
			}

			//Short jump
			if (vsp < 0 && inp_ButtonReleased(BTN_JUMP))
				vsp = 0;

			//Animate walk
			if (hsp != 0)
				imageIndex = fmod(imageIndex + 0.1, 2);

			//charging
			if (canCharge && inp_ButtonHeld(BTN_ATTACK))
			{
				chargeTimer += 1;
				//create effects
				if (chargeTimer >= 10 && chargeTimer < 66 && ((chargeTimer - 10) % 8) == 0)
					effectList.add(new ChargeOrb());

				if (chargeTimer == 70)
					aud_PlaySound(sounds[sndPower01]);
			}

			if (canCharge && chargeTimer >= 70 && inp_ButtonReleased(BTN_ATTACK))
			{
				state = HERO_STATE_CHARGE;
				chargeTimer = 0;
				imageIndex = 0;
				weaponList.add(new Sword(x,y));
			}

			//Attack
			if (stunTimer <= 0)
			{
				//Sword
				if (inp_ButtonPressed(BTN_ATTACK))
				{
					state = HERO_STATE_SLASH;
					imageIndex = 0;

					weaponList.add(new Sword(x,y));
				}

				//Weapons
				if (weapon != 0 && inp_ButtonPressed(BTN_WEAPON))
				{
					if (ammo > 0 && (weaponList.count() + (weapon == 5) < maxProjectiles[gameHasItem(ITEM_GREEN_SCROLL)]))
					{
						ammo -= 1;

						switch (weapon)
						{
							case 1: weaponList.add(new Arrow(x,y,direction)); break;
							case 2: weaponList.add(new Axe(x,y,direction)); break;
							case 3: weaponList.add(new Boomerang(x,y,direction)); break;
							case 4: weaponList.add(new Fireball(x,y,direction)); break;
							case 5: weaponList.add(new Bomb(x,y,direction)); break;
						}
					}
				}
			}

			//Climb down onto ladder
			if (onGround && inp_ButtonHeld(BTN_DOWN))
			{
				Rectangle collide;
				if (mapCollision(x,y+10,TILE_LADDER,&collide))
				{
					state = HERO_STATE_LADDER;
					canWalk = false;
					hsp = 0;
					vsp = 0;
					setX(collide.x + 10);
					y += 1;
				}
			}
			//Grab Ladder
			else if (inp_ButtonHeld(BTN_UP)/* || inp_ButtonHeld(BTN_DOWN)*/)
			{
				Rectangle collide;
				if (mapCollision(x,y,TILE_LADDER,&collide))
				{
					state = HERO_STATE_LADDER;
					canWalk = false;
					hsp = 0;
					vsp = 0;
					x = collide.x + 10;
				}
			}
		}

		else if (state == HERO_STATE_SLASH)
		{
			shieldTimer = 10;

			//Can move in air, but not on the ground
			if (onGround)
			{
				canWalk = false;
				hsp = 0;
			}
			else
			{
				//Short jump
				if (vsp < 0 && inp_ButtonReleased(BTN_JUMP))
					vsp = 0;
			}

			//Animate
			float imgspd = 0.25;

			if (imageIndex < 1) imgspd = 0.25;
			else if (imageIndex < 3) imgspd = 0.34;
			else if (imageIndex < 4) imgspd = 0.125;
			else if (imageIndex < 5) imgspd = 0.5;

			imageIndex += imgspd;

			if (imageIndex == 1)
				aud_PlaySound(sounds[sndShot01]);

			//Finish slash
			if (imageIndex >= 5)
			{
				state = HERO_STATE_NORMAL;
				canCharge = gameHasItem(ITEM_RED_SCROLL);
				chargeTimer = 0;
			}
		}

		else if (state == HERO_STATE_LADDER)
		{
			onGround = false;
			canWalk = false;
			canGrav = false;
			chargeTimer = 0;
			hsp = 0;
			vsp = 0;

			int yaxis = inp_ButtonHeld(BTN_DOWN) - inp_ButtonHeld(BTN_UP);

			//Movement
			y += climbSpeed[gameHasItem(ITEM_POWER_BAND)] * yaxis;

			//Animate
			if (yaxis)
				imageIndex = fmod(imageIndex + 0.125, 8);

			Rectangle mask = getMask();

			if (yaxis > 0)
			{
				Rectangle collide;

				//Touch ground
				if (mapCollision(mask, TILE_SOLID, &collide))
				{
					if (!mapCollision(x, y+10, TILE_LADDER))
					{
						state = HERO_STATE_NORMAL;
						y = collide.y - 10;
						onGround = true;
						imageIndex = 0;
					}
				}
				//Fall off of ladder
				else if (!mapCollision(mask, TILE_LADDER))
				{
					state = HERO_STATE_NORMAL;
					onGround = false;
					imageIndex = 0;
				}
			}
			//top of ladder
			else if (yaxis < 0)
			{
				if (!mapCollision(mask, TILE_LADDER))
				{
					state = HERO_STATE_NORMAL;
					onGround = false;

					Rectangle collide;
					mask.y += 5;

					if (mapCollision(mask, TILE_SOLID, &collide))
					{
						setY(collide.y - 10);
						onGround = true;
					}										
				}
			}
		}

		//Walking
		if (canWalk)
		{
			hsp = walkSpeed * (inp_ButtonHeld(BTN_RIGHT) - inp_ButtonHeld(BTN_LEFT));
		}
	}

	//Horizontal Movement
	if (hsp)
	{
		float thishsp = hsp;

		//Water / stun, slows movement
		if ((inWater && !gameHasItem(ITEM_FIN)) || stunTimer > 0)
		{
			thishsp /= 4.f;
		}
		//Water movement upgrade
		else if (inWater && gameHasItem(ITEM_FIN))
		{
			thishsp = thishsp / 3.f * 2.f;
		}		

		//Move
		x += thishsp;

		//Collide with wall
		Rectangle collide;
		Rectangle mask = getMask();

		if (solidCollision(mask, &collide))
		{
			if (hsp > 0)
				x = collide.x - mask.w / 2;
			else if (hsp < 0)
				x = collide.x + collide.w + mask.w / 2;
			
			if (state == HERO_STATE_STONE)
				direction *= -1;
		}
	}

	//Check if walked off of ledge
	if (onGround == true && vsp >= 0)
	{
		Rectangle mask = getMask();
		mask.y += 1;

		if (!solidCollision(mask) && !mapCollision(mask, TILE_LAVA))
		{
			onGround = false;

			//check platforms
			Rectangle pMask = (Rectangle){ 0, 0, 20, 20 };
			Object* p = platformList.next(nullptr);
			while (p != nullptr)
			{
				int f = p->getSpawnFlag();

				if (f == 0 || gameGetFlag(f))
				{
					pMask.x = p->getX();
					pMask.y = p->getY();

					if (collision(mask, pMask))
					{
						onGround = true;
						break;
					}
				}

				p = platformList.next(p);
			}						
		}
	}

	//Gravity
	if (canGrav && !onGround)
	{
		float maxvsp = 4.f;

		//Water slows movement
		if (inWater)
		{
			grav /= 2.f;
			maxvsp /= 2.f;
		}

		vsp += grav;

		if (vsp > maxvsp)
			vsp = maxvsp;
	}

	//Vertical Movement
	if (vsp)
	{
		float thisvsp = vsp;
		bool landed = false;

		//Water slows movement
		if (inWater || stunTimer > 0)
			thisvsp /= 2.f;

		//Movement
		y += thisvsp;

		//Collide with floor/ceiling
		Rectangle collide;
		Rectangle mask = getMask();

		if (solidCollision(mask, &collide) || (gameHasItem(ITEM_RED_BOOTS) && mapCollision(mask, TILE_LAVA, &collide)))
		{
			//floor
			if (vsp > 0)
			{
				y = collide.y - 10.f;
				vsp = 0;
				onGround = true;
				landed = true;
			}

			//ceiling
			else if (vsp < 0)
			{
				y = collide.y + 20 + mask.h - 10;
			}
		}
		//Platform objects
		else
		{
			if (vsp >= 0)
			{
				Rectangle altMask = (Rectangle){ mask.x, mask.y, mask.w, mask.h };
				altMask.y = mask.y - vsp;
				Rectangle pMask = (Rectangle){ 0, 0, 20, 20 };
				Object* p = platformList.next(nullptr);
				while (p != nullptr)
				{
					if (!p->isSolid())
					{
						pMask.x = p->getX();
						pMask.y = p->getY();

						int f = p->getSpawnFlag();

						if (f == 0 || gameGetFlag(f))
						{
							if (collision(mask, pMask) && !collision(altMask, pMask))
							{
								y = p->getY() - 10;
								vsp = 0;
								onGround = true;
								landed = true;
								break;
							}
						}
					}

					p = platformList.next(p);
				}
			}
		}

		//Land on ground after a hit
		if (landed)
		{
			onGround = true;
			vsp = 0;

			if (state == HERO_STATE_HIT || state == HERO_STATE_STONE)
			{
				hitTimer = 60;
				aud_PlaySound(sounds[sndHit01]);
				//create effects
				effectList.add(new Dust(x-15,y-6,-1));
				effectList.add(new Dust(x-5, y-6, 1));
			}
		}
	}

	//earthquake
	if (onGround)
	{
		canDoubleJump = true;

		if (state != HERO_STATE_QUAKE && gameGetQuake() > 0 && !gameHasItem(ITEM_CATFISH_GEM))
		{
			state = HERO_STATE_QUAKE;
			quakeCounter = 0;
		}
	}

	//collide with spikes
	Rectangle spike;
	if (mapCollision(getMask(), TILE_SPIKE, &spike))
	{
		spike.x += 5;
		spike.y += 5;
		spike.w = 10;
		spike.h = 10;

		hit(spike, 15);
	}

	//collide with lava
	if (mapCollision(x, y-10, TILE_LAVA))
	{
		hp = 0;
		kill();
	}
}

void Hero::draw(float subFrame, float depth)
{
	float dx = lerp(x, prevx, subFrame);
	float dy = lerp(y, prevy, subFrame);

	if (invincibleTimer % 2 == 0)
	{
		bool drawShield = false;
		int tile = 0;

		switch (state)
		{
		case HERO_STATE_LADDER:
		{
			int animation[] = { 0, 1, 2, 1, 0, 3, 4, 3 };
			tile = 34 + animation[(int)imageIndex % 8];
		}
		break;

		case HERO_STATE_NORMAL:
		{
			if (onGround)
			{
				//Walking
				if (hsp != 0)
				{
					tile = (int)imageIndex + ((direction == -1)? 2: 0);
				}
				//Standing
				else
				{
					imageIndex = 0;

					tile = 0 + ((direction == -1)? 2: 0);

					//Shield stance
					if (gameHasItem(ITEM_SHIELD) && shieldTimer <= 0)
					{
						drawShield = true;
						tile = 48 + (direction == -1) + ((heldUp)? 2: 0);
					}
				}
			}
			//Jumping and falling
			else
			{
				tile = 4 + (vsp >= 0) + ((direction == -1)? 2: 0);
			}
		}
		break;

		case HERO_STATE_SLASH:
		case HERO_STATE_CHARGE:
		{
			int animation[] = { 0, 1, 2, 2, 0 };
			tile = 16 + animation[(int)imageIndex % 5] + ((direction == -1)? 3: 0);
		}
		break;

		case HERO_STATE_HIT:
		{
			tile = 8 + 4 * onGround;
			tile += (int)imageIndex + ((direction == -1) * 2);
		}
		break;

		case HERO_STATE_STONE:
		{
			//frozen
			if (stoneState != 2)
				tile = 28 + (int)imageIndex + ((direction == -1) * 2);
			//break free
			else
			{
				int animation[] = { 0, 1, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 0, 1, 0 };
				tile = 24 + animation[(int)imageIndex];
			}
		}
		break;

		case HERO_STATE_QUAKE:
		{
			tile = 32 + ((direction == -1)? 1: 0);
		}
		break;

		case HERO_STATE_DEATH:
		{
			if (deathTimer >= 130)
				drawTextBold("GAME OVER", 124 + (-8*depth), 120, 2);

			//don't draw character
			if (deathTimer >= 90)
				return;

			if (direction == 1)
			{
				int animation[] = { 0, 3, 6, 9 };
				tile = animation[(int)imageIndex % 4];
			}
			else
			{
				int animation[] = { 2, 1, 4, 11 };
				tile = animation[(int)imageIndex % 4];
			}
		}
		break;

		case HERO_STATE_ITEM_GET:
		{
			int animation[] = { 0, 1, 2, 3, 2, 0, 1 };
			tile = 24 + animation[(int)imageIndex];
		}
		break;

		case HERO_STATE_DOOR:
		{
			tile = 64 + (int)imageIndex;
		}
		break;

		default: //warning nag prevention
		break;
		}

		gfx_DrawTile(images[imgMychr], dx-10, dy-10, 20, 20, tile);	

		if (drawShield)
		{
			float sx = dx + 9 - ((direction == -1)? 18: 0) - ((heldUp)? 4 * direction: 0);
			float sy = dy + 5 - ((heldUp)? 13: 0);

			if (!heldUp)
				sx += -1 * depth;

			tile = 104 + (direction == -1) + ((heldUp)? 2: 0);
			gfx_DrawTile(images[imgMychr], sx-10, sy-10, 20, 20, tile);	
		}
	}

	//stun effect
	if (stunTimer > 0)
	{
		int frame = ((300 - stunTimer) % 32) / 4;
		int animation[] = { 0, 1, 2, 1, 0, -1, -1, -1 };

		if (animation[frame] != -1)
			gfx_DrawTile(images[imgChr32], dx-16, dy-16, 32, 32, 16 + animation[frame]);
	}

	//Rectangle r = getShieldMask();
	//gfx_DrawRect(r.x, r.y, r.x+r.w, r.y+r.h, (Color){ 0xFF, 0, 0 } );
}

bool Hero::canHit()
{
	if (invincibleTimer > 0)
		return false;

	if (state == HERO_STATE_HIT || state == HERO_STATE_DEATH || state == HERO_STATE_DOOR)
		return false;

	if (state == HERO_STATE_STONE && (stoneState == 0 || stoneState == 2))
		return false;

	return true;
}

void Hero::_hit(int damage, int x)
{
	aud_PlaySound(sounds[sndHit02]);

	vsp = -2;
	onGround = false;

	//jump out of solid block
	Rectangle collide;
	if (solidCollision(getMask(), &collide))
	{
		if (y >= collide.y+10)
			y = collide.y+30;
		else
			y = collide.y-10;
	}

	//change state
	if (state == HERO_STATE_STONE)
		stoneState = 0;
	else
		state = HERO_STATE_HIT;

	//subtract health
	if (hp > (unsigned long)damage)
		hp -= damage;
	else
		hp = 0;

	//fly backwards
	direction = ((this->x < x)? 1: -1);
	hsp = direction * -1;
}

bool Hero::hit(Rectangle mask, int damage)
{
	if (!canHit())
		return false;

	if (collision(getMask(), mask))
	{
		_hit(damage, mask.x+(mask.w/2));
		return true;
	}

	return false;
}

bool Hero::hit(Circle mask, int damage)
{
	if (!canHit())
		return false;

	if (collision(getMask(), mask))
	{
		_hit(damage, mask.x);
		return true;
	}

	return false;
}

bool Hero::collide(Rectangle r)
{
	return collision(r, getMask());
}

bool Hero::collide(Circle c)
{
	return collision(c, getMask());
}

void Hero::kill()
{
	if (state == HERO_STATE_DEATH)
		return;

	aud_StopMusic();
	
	state = HERO_STATE_DEATH;
	deathTimer = 0;
}

void Hero::stun()
{
	if (gameHasItem(ITEM_CAPE))
		return;

	if (stoneTimer > 0)
		return;

	if (stunTimer <= 0)
		stunTimer = 300;
}

void Hero::petrify()
{
	if (gameHasItem(ITEM_GREEN_RING))
		return;

	if (stunTimer > 0)
		return;

	state = HERO_STATE_STONE;
	stoneState = 0;

	if (stoneTimer <= 0)
		stoneTimer = 350;
}

void Hero::poison()
{
	//purple ring prevents poison
	if (gameHasItem(ITEM_PURPLE_RING))
		return;

	if (poisonTimer <= 0)
		poisonTimer = 300;
}

void Hero::changeWeapon()
{
	int axis = inp_ButtonPressed(BTN_R) - inp_ButtonPressed(BTN_L);

	if (axis)
	{
		aud_PlaySound(sounds[sndPi01]);
		int newWeapon = weapon;

		for (int i = 0; i < 5; i++)
		{
			newWeapon += axis;

			if (newWeapon > 5) newWeapon = 1;
			if (newWeapon <= 0) newWeapon = 5;

			if (gameHasWeapon(newWeapon))
			{
				weapon = newWeapon;
				break;
			}
		}
	}
}

Rectangle Hero::getMask()
{
	Rectangle r;

	r.x = x - 6;
	r.y = y - 3;
	r.w = 12;
	r.h = 13;

	return r;
}

Rectangle Hero::getShieldMask()
{
	Rectangle r;

	r.x = -320;
	r.y = -240;
	r.w = 0;
	r.h = 0;

	if (gameHasItem(ITEM_SHIELD) && state == HERO_STATE_NORMAL && onGround && hsp == 0 && shieldTimer <= 0)
	{
		if (heldUp)
		{
			//above head
			r.x = x - 1 - ((direction == -1)? 10: 0);
			r.y = y - 10;
			r.w = 12;
			r.h = 4;
		}
		else
		{
			//normal
			r.x = x + 5 - ((direction == -1)? 17: 0);
			r.y = y;
			r.w = 7;
			r.h = 10;
		}
	}

	return r;
}

void Hero::setState(HeroState hs)
{
	if (hs == HERO_STATE_ITEM_GET)
	{
		imageIndex = 0;
		hsp = 0;
		vsp = 0;
	}

	state = hs;
}

void Hero::setWeapon(int n)
{
	if (gameHasWeapon(n))
		weapon = n;
}