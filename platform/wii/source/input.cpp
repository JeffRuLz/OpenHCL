#include "input.hpp"
#include "callbacks.hpp"
#include "system.hpp"
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiidrc/wiidrc.h>
#include <math.h>

static unsigned int kBuffer = 0;
static unsigned int kDown = 0;
static unsigned int kDownPrev = 0;

int inp_Init()
{
	PAD_Init();
	WPAD_Init();
	WiiDRC_Init();

	WPAD_SetPowerButtonCallback(powerCallback);
	
	return 0;
}

void inp_Exit()
{

}

static void _scanGamecube(u16 down)
{
	s8 stickDeadZone = 32;
	u8 triggerDeadZone = 128;

	if (PAD_StickY(0) > stickDeadZone  || (down & PAD_BUTTON_UP)) 	 kBuffer |= BTN_UP;
	if (PAD_StickY(0) < -stickDeadZone || (down & PAD_BUTTON_DOWN))  kBuffer |= BTN_DOWN;
	if (PAD_StickX(0) < -stickDeadZone || (down & PAD_BUTTON_LEFT))  kBuffer |= BTN_LEFT;
	if (PAD_StickX(0) > stickDeadZone  || (down & PAD_BUTTON_RIGHT)) kBuffer |= BTN_RIGHT;

	if (down & PAD_BUTTON_A) 	  kBuffer |= BTN_JUMP;
	if (down & PAD_BUTTON_B) 	  kBuffer |= BTN_ATTACK;
	if (down & PAD_BUTTON_X) 	  kBuffer |= BTN_WEAPON;
	if (down & PAD_BUTTON_START)  kBuffer |= BTN_SELECT;
	if (down & PAD_BUTTON_Y)	  kBuffer |= BTN_START;
	if (down & PAD_BUTTON_A) 	  kBuffer |= BTN_ACCEPT;
	if (down & PAD_BUTTON_B) 	  kBuffer |= BTN_DECLINE;
	if (down & PAD_TRIGGER_Z)	  kBuffer |= BTN_R;

	if (PAD_TriggerL(0) > triggerDeadZone || (down & PAD_TRIGGER_L)) kBuffer |= BTN_L;
	if (PAD_TriggerR(0) > triggerDeadZone || (down & PAD_TRIGGER_R)) kBuffer |= BTN_R;
}

static void _scanWiimote(u32 down)
{
	if (down & WPAD_BUTTON_RIGHT) kBuffer |= BTN_UP;
	if (down & WPAD_BUTTON_LEFT)  kBuffer |= BTN_DOWN;
	if (down & WPAD_BUTTON_UP) 	  kBuffer |= BTN_LEFT;
	if (down & WPAD_BUTTON_DOWN)  kBuffer |= BTN_RIGHT;
	if (down & WPAD_BUTTON_2)	  kBuffer |= BTN_JUMP;
	if (down & WPAD_BUTTON_1) 	  kBuffer |= BTN_ATTACK;
	if (down & WPAD_BUTTON_B) 	  kBuffer |= BTN_WEAPON;
	if (down & WPAD_BUTTON_PLUS)  kBuffer |= BTN_START;
	if (down & WPAD_BUTTON_HOME)  kBuffer |= BTN_SELECT;
	if (down & WPAD_BUTTON_2) 	  kBuffer |= BTN_ACCEPT;
	if (down & WPAD_BUTTON_1)     kBuffer |= BTN_DECLINE;
	if (down & WPAD_BUTTON_A)	  kBuffer |= BTN_R;
	if (down & WPAD_BUTTON_MINUS) kBuffer |= BTN_R;
}

//A big chunk of these two functions are from sdl-wii
static s16 WPAD_StickX(int chan)
{
	float mag = 0.0;
	float ang = 0.0;

	WPADData* data = WPAD_Data(chan);
	
	expansion_t exp;
	WPAD_Expansion(chan, &exp);

	switch (data->exp.type)
	{
		case EXP_NUNCHUK:
			mag = data->exp.nunchuk.js.mag;
			ang = data->exp.nunchuk.js.ang;
			break;

		case EXP_CLASSIC:
			mag = data->exp.classic.ljs.mag;
			ang = data->exp.classic.ljs.ang;
			break;
	}

	if (mag > 1.0) mag = 1.0;
	else if (mag < -1.0) mag = -1.0;

	return (mag * sin( (3.14159 * ang)/180.0) ) * 128.0;
}

static s16 WPAD_StickY(int chan)
{
	float mag = 0.0;
	float ang = 0.0;

	WPADData* data = WPAD_Data(chan);
	
	expansion_t exp;
	WPAD_Expansion(chan, &exp);

	switch (data->exp.type)
	{
		case EXP_NUNCHUK:
			mag = data->exp.nunchuk.js.mag;
			ang = data->exp.nunchuk.js.ang;
			break;

		case EXP_CLASSIC:
			mag = data->exp.classic.ljs.mag;
			ang = data->exp.classic.ljs.ang;
			break;
	}

	if (mag > 1.0) mag = 1.0;
	else if (mag < -1.0) mag = -1.0;

	return -(mag * cos( (3.14159 * ang)/180.0) ) * 128.0;
}

static void _scanClassic(u32 down)
{
	s8 stickDeadZone = 32;

	if (WPAD_StickY(0) < -stickDeadZone || (down & WPAD_CLASSIC_BUTTON_UP))    kBuffer |= BTN_UP;
	if (WPAD_StickY(0) >  stickDeadZone || (down & WPAD_CLASSIC_BUTTON_DOWN))  kBuffer |= BTN_DOWN;
	if (WPAD_StickX(0) < -stickDeadZone || (down & WPAD_CLASSIC_BUTTON_LEFT))  kBuffer |= BTN_LEFT;
	if (WPAD_StickX(0) >  stickDeadZone || (down & WPAD_CLASSIC_BUTTON_RIGHT)) kBuffer |= BTN_RIGHT;

	if (down & WPAD_CLASSIC_BUTTON_B) 	  kBuffer |= BTN_JUMP;
	if (down & WPAD_CLASSIC_BUTTON_Y) 	  kBuffer |= BTN_ATTACK;
	if (down & WPAD_CLASSIC_BUTTON_A) 	  kBuffer |= BTN_WEAPON;
	if (down & WPAD_CLASSIC_BUTTON_PLUS)  kBuffer |= BTN_START;
	if (down & WPAD_CLASSIC_BUTTON_X)	  kBuffer |= BTN_START;
	if (down & WPAD_CLASSIC_BUTTON_HOME)  kBuffer |= BTN_SELECT;
	if (down & WPAD_CLASSIC_BUTTON_A) 	  kBuffer |= BTN_ACCEPT;
	if (down & WPAD_CLASSIC_BUTTON_B)     kBuffer |= BTN_DECLINE;
	if (down & WPAD_CLASSIC_BUTTON_MINUS) kBuffer |= BTN_R;
	
	if ((down & WPAD_CLASSIC_BUTTON_FULL_L) || (down & WPAD_CLASSIC_BUTTON_ZL)) kBuffer |= BTN_L;
	if ((down & WPAD_CLASSIC_BUTTON_FULL_R) || (down & WPAD_CLASSIC_BUTTON_ZR)) kBuffer |= BTN_R;
}

static void _scanNunchuk(u32 down)
{
	s8 stickDeadZone = 32;

	if (WPAD_StickY(0) < -stickDeadZone) kBuffer |= BTN_UP;
	if (WPAD_StickY(0) >  stickDeadZone) kBuffer |= BTN_DOWN;
	if (WPAD_StickX(0) < -stickDeadZone) kBuffer |= BTN_LEFT;
	if (WPAD_StickX(0) >  stickDeadZone) kBuffer |= BTN_RIGHT;
	
	if (down & WPAD_BUTTON_A) 	  	  kBuffer |= BTN_JUMP;
	if (down & WPAD_BUTTON_B) 	  	  kBuffer |= BTN_ATTACK;
	if (down & WPAD_NUNCHUK_BUTTON_Z) kBuffer |= BTN_WEAPON;
	if (down & WPAD_BUTTON_PLUS)	  kBuffer |= BTN_START;
	if (down & WPAD_BUTTON_HOME)	  kBuffer |= BTN_SELECT;
	if (down & WPAD_BUTTON_A) 	 	  kBuffer |= BTN_ACCEPT;
	if (down & WPAD_BUTTON_B)     	  kBuffer |= BTN_DECLINE;
	if (down & WPAD_BUTTON_MINUS)	  kBuffer |= BTN_R;
	if (down & WPAD_NUNCHUK_BUTTON_C) kBuffer |= BTN_R;

	if (down & WPAD_BUTTON_LEFT)  kBuffer |= BTN_L;
	if (down & WPAD_BUTTON_RIGHT) kBuffer |= BTN_R;
}

static void _scanWiiDRC(u32 down)
{
	s8 stickDeadZone = 32;

	if(WiiDRC_lStickY() >  stickDeadZone || (down & WIIDRC_BUTTON_UP))		kBuffer |= BTN_UP;
	if(WiiDRC_lStickY() < -stickDeadZone || (down & WIIDRC_BUTTON_DOWN))	kBuffer |= BTN_DOWN;
	if(WiiDRC_lStickX() < -stickDeadZone || (down & WIIDRC_BUTTON_LEFT))	kBuffer |= BTN_LEFT;
	if(WiiDRC_lStickX() >  stickDeadZone || (down & WIIDRC_BUTTON_RIGHT))	kBuffer |= BTN_RIGHT;

	if(down & WIIDRC_BUTTON_B)		kBuffer |= BTN_JUMP;
	if(down & WIIDRC_BUTTON_Y)		kBuffer |= BTN_ATTACK;
	if(down & WIIDRC_BUTTON_A)		kBuffer |= BTN_WEAPON;
	if(down & WIIDRC_BUTTON_PLUS)	kBuffer |= BTN_START;
	if(down & WIIDRC_BUTTON_X)		kBuffer |= BTN_START;
	if(down & WIIDRC_BUTTON_HOME)	kBuffer |= BTN_SELECT;
	if(down & WIIDRC_BUTTON_A)		kBuffer |= BTN_ACCEPT;
	if(down & WIIDRC_BUTTON_B)		kBuffer |= BTN_DECLINE;
	if(down & WIIDRC_BUTTON_MINUS)	kBuffer |= BTN_R;

	if((down & WIIDRC_BUTTON_L) || (down & WIIDRC_BUTTON_ZL))	kBuffer |= BTN_L;
	if((down & WIIDRC_BUTTON_R) || (down & WIIDRC_BUTTON_ZR))	kBuffer |= BTN_R;
}

void inp_Scan()
{
	PAD_ScanPads();
	u16 gDown = PAD_ButtonsHeld(0);

	WPAD_ScanPads();
	u32 wDown = WPAD_ButtonsHeld(0);

	expansion_t exp;
	WPAD_Expansion(0, &exp);

	//
	kBuffer = 0;

	_scanGamecube(gDown);

	switch (exp.type)
	{
		case EXP_CLASSIC: _scanClassic(wDown); break;
		case EXP_NUNCHUK: _scanNunchuk(wDown); break;
		default: _scanWiimote(wDown); break;
	}

	if(WiiDRC_Inited() && WiiDRC_Connected())
	{
		WiiDRC_ScanPads();
		u32 uDown = WiiDRC_ButtonsHeld();
		_scanWiiDRC(uDown);
	}

	//disable opposites
	if ( (kBuffer & BTN_UP) && (kBuffer & BTN_DOWN) )
		kBuffer ^= BTN_UP | BTN_DOWN;

	if ( (kBuffer & BTN_LEFT) && (kBuffer & BTN_RIGHT) )
		kBuffer ^= BTN_LEFT | BTN_RIGHT;
}

void inp_Update()
{
	kDownPrev = kDown;
	kDown = kBuffer;
}

bool inp_ButtonHeld(unsigned int btn)
{
	if (kDown & btn)
		return true;
	
	return false;
}

bool inp_ButtonPressed(unsigned int btn)
{
	return !(kDownPrev & btn) && (kDown & btn);
}

bool inp_ButtonReleased(unsigned int btn)
{
	return (kDownPrev & btn) && !(kDown & btn);
}