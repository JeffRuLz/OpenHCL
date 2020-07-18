#include "input.hpp"

#include <pspctrl.h>
#include "registry.h"

static unsigned int kBuffer = 0;
static unsigned int kDown = 0;
static unsigned int kDownPrev = 0;

static unsigned int acceptButton = PSP_CTRL_CROSS;
static unsigned int declineButton = PSP_CTRL_CIRCLE;

static SceCtrlData pad;

int inp_Init()
{
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	unsigned int btn;
	if(get_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", &btn))
	{
		if (btn == 0)
		{
			acceptButton = PSP_CTRL_CIRCLE;
			declineButton = PSP_CTRL_CROSS;
		}
		else
		{
			acceptButton = PSP_CTRL_CROSS;
			declineButton = PSP_CTRL_CIRCLE;
		}
	}

	return 0;
}

void inp_Exit()
{

}

void inp_Scan()
{
	sceCtrlReadBufferPositive(&pad, 1);

	int deadzone = 32;

	kBuffer = 0;
	if (pad.Ly < 128-deadzone || (pad.Buttons & PSP_CTRL_UP))    kBuffer |= BTN_UP;
	if (pad.Ly > 128+deadzone || (pad.Buttons & PSP_CTRL_DOWN))  kBuffer |= BTN_DOWN;
	if (pad.Lx < 128-deadzone || (pad.Buttons & PSP_CTRL_LEFT))  kBuffer |= BTN_LEFT;
	if (pad.Lx > 128+deadzone || (pad.Buttons & PSP_CTRL_RIGHT)) kBuffer |= BTN_RIGHT;

	if ((pad.Buttons & PSP_CTRL_CROSS))    kBuffer |= BTN_JUMP;
	if ((pad.Buttons & PSP_CTRL_SQUARE))   kBuffer |= BTN_ATTACK;
	if ((pad.Buttons & PSP_CTRL_CIRCLE))   kBuffer |= BTN_WEAPON;
	if ((pad.Buttons & PSP_CTRL_START))    kBuffer |= BTN_START;
	if ((pad.Buttons & PSP_CTRL_TRIANGLE)) kBuffer |= BTN_START;
	if ((pad.Buttons & PSP_CTRL_SELECT))   kBuffer |= BTN_SELECT;
	if ((pad.Buttons & acceptButton)) 	   kBuffer |= BTN_ACCEPT;
	if ((pad.Buttons & declineButton)) 	   kBuffer |= BTN_DECLINE;
	if ((pad.Buttons & PSP_CTRL_LTRIGGER)) kBuffer |= BTN_L;
	if ((pad.Buttons & PSP_CTRL_RTRIGGER)) kBuffer |= BTN_R;

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
	return (kDown & btn);
}

bool inp_ButtonPressed(unsigned int btn)
{
	return !(kDownPrev & btn) && (kDown & btn);
}

bool inp_ButtonReleased(unsigned int btn)
{
	return (kDownPrev & btn) && !(kDown & btn);
}