#include "input.hpp"
#include <3ds.h>

static unsigned int kBuffer = 0;
static unsigned int kDown = 0;
static unsigned int kDownPrev = 0;

static touchPosition touch;

int inp_Init()
{	
	return 0;
}

void inp_Exit()
{

}

void inp_Scan()
{
	hidTouchRead(&touch);
	hidScanInput();
	u32 kHeld = hidKeysHeld();

	kBuffer = 0;
	if (kHeld & KEY_UP) 	kBuffer |= BTN_UP;
	if (kHeld & KEY_DOWN) 	kBuffer |= BTN_DOWN;
	if (kHeld & KEY_LEFT) 	kBuffer |= BTN_LEFT;
	if (kHeld & KEY_RIGHT) 	kBuffer |= BTN_RIGHT;
	if (kHeld & KEY_B) 		kBuffer |= BTN_JUMP;
	if (kHeld & KEY_Y) 		kBuffer |= BTN_ATTACK;
	if (kHeld & KEY_A) 		kBuffer |= BTN_WEAPON;
	if (kHeld & KEY_X)		kBuffer |= BTN_START;
	if (kHeld & KEY_START) 	kBuffer |= BTN_START;
	if (kHeld & KEY_SELECT) kBuffer |= BTN_SELECT;
	if (kHeld & KEY_A) 		kBuffer |= BTN_ACCEPT;
	if (kHeld & KEY_B) 		kBuffer |= BTN_DECLINE;

	if ((kHeld & KEY_L) || (kHeld & KEY_ZL)) kBuffer |= BTN_L;
	if ((kHeld & KEY_R) || (kHeld & KEY_ZR)) kBuffer |= BTN_R;
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

float inp_Get3DSlider()
{
	return osGet3DSliderState();
}

int inp_GetTouchX()
{
	return touch.px;
}

int inp_GetTouchY()
{
	return touch.py;
}