#ifndef __WadeWork__sys_h__
#define __WadeWork__sys_h__

namespace ww
{
	namespace sys
	{
		const unsigned int CONFIG_OPENGL1 = 0x1;
		const unsigned int CONFIG_OPENGL2 = 0x2;
		const unsigned int CONFIG_DISABLE_OPENGL_DEPTHBUFFER = 0x4;
		const unsigned int CONFIG_CUSTOM_TIMER = 0x8;
		const unsigned int CONFIG_XINPUT = 0x10;
		const unsigned int CONFIG_DISABLE_OPENAL = 0x20;
		const unsigned int CONFIG_WINSOCK_ONLY = 0x40;

		extern int setup(unsigned int config);
		extern int setup_apple(unsigned int config, const char *path, int argc, char * argv[]);

		extern void setInitCallback(bool (*funcPtr)(void));
		extern void setTimerCallback(bool (*funcPtr)(void));
		extern void setDeinitCallback(bool (*funcPtr)(void));
		extern void setBackgroundCallback(bool (*funcPtr)(void));
		extern void setForegroundCallback(bool (*funcPtr)(void));

		extern void setTimerResolution(unsigned int resolution);
		extern unsigned getTimerResolution();

		extern void customTimer_timerBegin();
		extern void customTimer_timerFinish();
		extern void customTimer_unsetup();
	};
};

#endif