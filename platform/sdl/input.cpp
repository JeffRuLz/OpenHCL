#include "input.hpp"
#include "system.hpp"
#include <SDL/SDL.h>
#include <stdio.h>

static unsigned int kBuffer = 0;
static unsigned int kDown = 0;
static unsigned int kDownPrev = 0;

//static SDL_GameController* pad = nullptr;
//static Sint16 deadzone = 32767 / 3;

int inp_Init()
{
	/*
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
*/
	return 0;
}

void inp_Exit()
{
	/*
	if (pad)
	{
		SDL_GameControllerClose(pad);
		pad = nullptr;
	}	

	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	*/
}

void inp_Scan()
{
	kBuffer = 0;

	//keyboard
	const Uint8* keystate = SDL_GetKeyState(nullptr);

	if (keystate[SDLK_UP])		kBuffer |= BTN_UP;
	if (keystate[SDLK_DOWN])	kBuffer |= BTN_DOWN;
	if (keystate[SDLK_LEFT])	kBuffer |= BTN_LEFT;
	if (keystate[SDLK_RIGHT])	kBuffer |= BTN_RIGHT;

	if (keystate[SDLK_LALT])	kBuffer |= BTN_JUMP | BTN_DECLINE;
	if (keystate[SDLK_LSHIFT])	kBuffer |= BTN_ATTACK;
	if (keystate[SDLK_LCTRL])	kBuffer |= BTN_WEAPON | BTN_ACCEPT;

	if (keystate[SDLK_RETURN])	kBuffer |= BTN_START | BTN_ACCEPT;
	if (keystate[SDLK_SPACE])	kBuffer |= BTN_START;
	if (keystate[SDLK_ESCAPE])	kBuffer |= BTN_SELECT;

	if (keystate[SDLK_TAB])		  kBuffer |= BTN_L;
	if (keystate[SDLK_BACKSPACE]) kBuffer |= BTN_R;
/*
	//controller
	if (pad)
	{
		SDL_GameControllerUpdate();

		Sint16 xaxis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
		Sint16 yaxis = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);

		if (yaxis < -deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP))
			kBuffer |= BTN_UP;
		if (yaxis > deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
			kBuffer |= BTN_DOWN;
		if (xaxis < -deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
			kBuffer |= BTN_LEFT;
		if (xaxis > deadzone || SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
			kBuffer |= BTN_RIGHT;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A))
			kBuffer |= BTN_JUMP | BTN_ACCEPT;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_B))
			kBuffer |= BTN_WEAPON | BTN_DECLINE;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X))
			kBuffer |= BTN_ATTACK;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_START))
			kBuffer |= BTN_START;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_BACK))
			kBuffer |= BTN_SELECT;

		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER) ||
			SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 0)
			kBuffer |= BTN_L;
		if (SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) ||
			SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0)
			kBuffer |= BTN_R;
	}
*/
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