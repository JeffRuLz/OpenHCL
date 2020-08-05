#include "system.hpp"
#include "graphics.hpp"
#include "audio.hpp"
#include "input.hpp"

#include "ini.hpp"
#include "game.hpp"
#include "text.hpp"

#ifdef _3DS
  #include "inventory.hpp"
#endif

#ifdef _NTSC
  #define FRAME_RATE 59.94f
#else
  #define FRAME_RATE 60.f
#endif

int main(int argc, char* argv[])
{
	sys_Init();
	gfx_Init();
	aud_Init();
	inp_Init();

	gameSetLanguage(sys_GetLanguage());
	iniLoad();
	loadText(gameGetLanguage());

	gfx_Start();
	
  #ifndef _NO_FRAMESKIP
	float msPerFrame = 1000.f / FRAME_RATE;
	float timeStack = 0.f;
	unsigned long time = sys_GetMillisecond();
	unsigned long lastTime = time;
  #endif
	float subFrame = 0.f;

	gameStart();

	while (sys_MainLoop())
	{
		inp_Scan();

	  #ifndef _NO_FRAMESKIP
		time = sys_GetMillisecond();
		timeStack += time - lastTime;
		lastTime = time;

		if (timeStack > msPerFrame * 3)
			timeStack = msPerFrame * 3;

		for (; timeStack > msPerFrame; timeStack -= msPerFrame)
	  #endif
		{
			inp_Update();
			gameUpdate();

		  #ifdef _3DS
			inventoryTouch();
		  #endif
		}

		float depth = 0;
	  	#ifdef _3DS
		depth = inp_Get3DSlider() * 0.8;
	  	#endif

		gfx_FrameStart();
		{
		  	#ifndef _NO_FRAMESKIP
			subFrame = timeStack / msPerFrame;
		  	#endif
			gameDraw(subFrame, -depth);

			#ifdef _3DS
				if (depth > 0.0f)
				{
					gfx_FrameSelect(1);
					gameDraw(subFrame, depth);
				}

				//bottom screen
			    #ifndef _DEBUG
					gfx_FrameSelect(2);
					inventoryDraw(subFrame, 0);
				#endif
			#endif
		}
		gfx_FrameEnd();
	}
	
	gameEnd();
	
	gfx_End();

	inp_Exit();
	aud_Exit();
	gfx_Exit();
	sys_Exit();

	return 0;
}