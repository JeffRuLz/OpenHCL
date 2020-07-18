/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

#include "callbacks.h"

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <psppower.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

static int exitRequest = 0;

int running()
{
	sceKernelDelayThread(10);
	return !exitRequest;
}

int exitCallback(int arg1, int arg2, void *common)
{
	exitRequest = 1;
	return 0;
}

int powerCallback(int unknown, int pwrflags, void* common)
{
	/* check for power switch and suspending as one is manual and the other automatic */
    if (pwrflags & PSP_POWER_CB_POWER_SWITCH || pwrflags & PSP_POWER_CB_SUSPENDING)
    {
		//sprintf(powerCBMessage, "first arg: 0x%08X, flags: 0x%08X: suspending\n", unknown, pwrflags);
		//suspendMusic();
    }
    else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE)
    {
		//sprintf(powerCBMessage, "first arg: 0x%08X, flags: 0x%08X: resume complete\n", unknown, pwrflags);
		//resumeMusic();
    }

    sceDisplayWaitVblankStart();

	return 0;
}

int callbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	cbid = sceKernelCreateCallback("Power Callback", powerCallback, NULL);
    scePowerRegisterCallback(0, cbid);

	sceKernelSleepThreadCB();

	return 0;
}

int setupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
		sceKernelStartThread(thid, 0, 0);

	return thid;
}
