#include "input.hpp"
#include "system.hpp"
#include <SDL2/SDL.h>
#include <stdio.h>
#ifdef _VITA
#include <psp2/apputil.h>
#include <psp2/system_param.h>
#endif

static unsigned int kBuffer = 0;
static unsigned int kDown = 0;
static unsigned int kDownPrev = 0;

static SDL_GameControllerButton acceptButton = SDL_CONTROLLER_BUTTON_A;
static SDL_GameControllerButton declineButton = SDL_CONTROLLER_BUTTON_B;

static SDL_GameController* pad = nullptr;
static Sint16 deadzone = 32767 / 3;

int inp_Init()
{
	int ret = SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

	if (ret < 0)
	{
		printf("Unable to initialize SDL Game Controller: %s\n", SDL_GetError());
		return ret;
	}

	pad = SDL_GameControllerOpen(0);

	if (!pad)
	{
		printf("No joystick found\n");
	}
	
#ifdef _VITA
	int enterButton = 0;
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, &enterButton);
	
	if (enterButton == SCE_SYSTEM_PARAM_ENTER_BUTTON_CROSS) {
		acceptButton = SDL_CONTROLLER_BUTTON_A;
		declineButton = SDL_CONTROLLER_BUTTON_B;
	}
	else {
		acceptButton = SDL_CONTROLLER_BUTTON_B;
		declineButton = SDL_CONTROLLER_BUTTON_A;
	}
#endif

	return 0;
}

void inp_Exit()
{
	if (pad)
	{
		SDL_GameControllerClose(pad);
		pad = nullptr;
	}	

	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void inp_Scan()
{
	kBuffer = 0;

	//keyboard
	const Uint8* keystate = SDL_GetKeyboardState(nullptr);

	if (keystate[SDL_SCANCODE_UP])		kBuffer |= BTN_UP;
	if (keystate[SDL_SCANCODE_DOWN])	kBuffer |= BTN_DOWN;
	if (keystate[SDL_SCANCODE_LEFT])	kBuffer |= BTN_LEFT;
	if (keystate[SDL_SCANCODE_RIGHT])	kBuffer |= BTN_RIGHT;

	if (keystate[SDL_SCANCODE_C])		kBuffer |= BTN_JUMP | BTN_ACCEPT;
	if (keystate[SDL_SCANCODE_X])		kBuffer |= BTN_ATTACK | BTN_DECLINE;
	if (keystate[SDL_SCANCODE_Z])		kBuffer |= BTN_WEAPON;

	if (keystate[SDL_SCANCODE_RETURN])	kBuffer |= BTN_START | BTN_ACCEPT;
	if (keystate[SDL_SCANCODE_ESCAPE])	kBuffer |= BTN_SELECT;

	if (keystate[SDL_SCANCODE_A])		kBuffer |= BTN_L;
	if (keystate[SDL_SCANCODE_S])		kBuffer |= BTN_R;

	//controller
	if (pad)
	{
		SDL_GameControllerUpdate();

		Sint16 xaxis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
		Sint16 yaxis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);

		if (yaxis < -deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP)) 	kBuffer |= BTN_UP;
		if (yaxis > deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))	kBuffer |= BTN_DOWN;
		if (xaxis < -deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) kBuffer |= BTN_LEFT;
		if (xaxis > deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))	kBuffer |= BTN_RIGHT;
		
		if (SDL_GameControllerGetButton(pad, acceptButton))		kBuffer |= BTN_ACCEPT;
		if (SDL_GameControllerGetButton(pad, declineButton))	kBuffer |= BTN_DECLINE;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A))	kBuffer |= BTN_JUMP;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B))	kBuffer |= BTN_WEAPON;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X))	kBuffer |= BTN_ATTACK;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START)) 	kBuffer |= BTN_START;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK))	kBuffer |= BTN_SELECT;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ||
			SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 0)
			kBuffer |= BTN_L;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ||
			SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0)
			kBuffer |= BTN_R;
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