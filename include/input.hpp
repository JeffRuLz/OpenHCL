#pragma once

enum
{
	BTN_UP		= (1 << 0),
	BTN_DOWN	= (1 << 1),
	BTN_LEFT	= (1 << 2),
	BTN_RIGHT	= (1 << 3),
	BTN_JUMP	= (1 << 4),
	BTN_ATTACK	= (1 << 5),
	BTN_WEAPON	= (1 << 6),
	BTN_START	= (1 << 7),
	BTN_SELECT	= (1 << 8),
	BTN_ACCEPT	= (1 << 9),
	BTN_DECLINE	= (1 << 10),
	BTN_L 		= (1 << 11),
	BTN_R 		= (1 << 12)	
};

int inp_Init();
void inp_Exit();

void inp_Scan();
void inp_Update();

bool inp_ButtonHeld(unsigned int btn);
bool inp_ButtonPressed(unsigned int btn);
bool inp_ButtonReleased(unsigned int btn);

#ifdef _3DS
float inp_Get3DSlider();
int inp_GetTouchX();
int inp_GetTouchY();
#endif