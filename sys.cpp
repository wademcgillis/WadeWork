#if PLATFORM_WINDOWS
#include <GL/glew.h>
#include <windows.h>
#include <gl/gl.h>
#endif
#if PLATFORM_OSX
#include <OpenGL/gl.h>
#endif

#include <WadeWork/wadework.h>
#include <SFML/Window.hpp>

#if PLATFORM_WINDOWS
#include <windows.h>
#include <mmsystem.h>
#include <SFML/OpenGL.hpp>
#include <winsock2.h>
#ifdef DEBUG
#pragma comment(lib,"sfml-system-s-d.lib")
#pragma comment(lib,"sfml-window-s-d.lib")
#else
#pragma comment(lib,"sfml-system-s.lib")
#pragma comment(lib,"sfml-window-s.lib")
#endif
#ifndef DISABLE_XINPUT
#pragma comment(lib,"xinput.lib")
#endif
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
#pragma comment(lib,"glew.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "openal32.lib")
#endif

namespace ww
{
	namespace gfx
	{
		extern sf::Window *window;
		extern unsigned int window_width;
		extern unsigned int window_height;
		extern void display();

		extern bool FORCE_GL1;
	}

	namespace sfx
	{
		extern ALCdevice *openal_device;
		extern ALCcontext *openal_context;
	}

	namespace input
	{
		namespace keyboard
		{
			extern void endOfFrame();
			extern void handleSfEvent(sf::Event event);
		}
		namespace mouse
		{
			extern void endOfFrame();
			extern void handleSfEvent(sf::Event event);
		}
		namespace xbox360
		{
			extern void endOfFrame();
			extern void handleEvent();
		}
	}

	namespace sys
	{
		bool (*initFuncPtr)(void);
		bool (*timerFuncPtr)(void);
		bool (*deinitFuncPtr)(void);
		bool (*backgroundFuncPtr)(void);
		bool (*foregroundFuncPtr)(void);

		unsigned int timerResolution;
		unsigned int CONFIG = 0;


		void setInitCallback(bool (*funcPtr)(void))
		{
			initFuncPtr = funcPtr;
		}
		void setTimerCallback(bool (*funcPtr)(void))
		{
			timerFuncPtr = funcPtr;
		}
		void setDeinitCallback(bool (*funcPtr)(void))
		{
			deinitFuncPtr = funcPtr;
		}
		void setBackgroundCallback(bool (*funcPtr)(void))
		{
			backgroundFuncPtr = funcPtr;
		}
		void setForegroundCallback(bool (*funcPtr)(void))
		{
			foregroundFuncPtr = funcPtr;
		}
		//ww::setForegroundCallback(NULL);
		//ww::setBackgroundCallback(NULL);

		void setTimerResolution(unsigned int resolution)
		{
			if (resolution == 0)
				resolution = 1;
			timerResolution = resolution;
#if PLATFORM_PC
			if (ww::gfx::window != NULL)
				ww::gfx::window->setFramerateLimit(timerResolution);
#elif PLATFORM_IOS
			//((GLKViewController*)glkcontroller).preferredFramesPerSecond = framerate;
#endif
		}

		unsigned int getTimerResolution()
		{
			return timerResolution;
		}

		int setup(unsigned int config)
		{
			CONFIG = config;
			ww::gui::initialize();
			if (CONFIG & ww::sys::CONFIG_OPENGL1)
			{
				ww::gfx::FORCE_GL1 = true;
				printf("HO HO HO OPENGL1!!!!!!\n");
			}
#if PLATFORM_WINDOWS
			timeBeginPeriod(1);
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
				fprintf(stderr, "WSAStartup failed.\n");
				exit(1);
			}
#endif
			unsigned int depthBufferBits = 0;
			
			depthBufferBits = 0;
			if (~CONFIG & ww::sys::CONFIG_DISABLE_OPENGL_DEPTHBUFFER)
				depthBufferBits = 24;
			ww::gfx::window = new sf::Window(sf::VideoMode(ww::gfx::window_width,ww::gfx::window_height,32),"",sf::Style::Close,sf::ContextSettings(depthBufferBits,0,0,1,1));
#if PLATFORM_WINDOWS
			if (CONFIG & ww::sys::CONFIG_OPENGL2)
			{
				printf("ABOUT  TO CALL!!!\n");
				if (ww::gfx::supportsOpenGL2())
					glewInit();
			}
#endif
			 
			glViewport(0,0,ww::gfx::window_width,ww::gfx::window_height);
			glEnable(GL_SCISSOR_TEST);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);//_MINUS_SRC_ALPHA);GL_SRC_ALPHA,GL_DST_ALPHA,GL_ONE,GL_ONE);//
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER,.01f);
			//glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);//GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW);

			if (~CONFIG & ww::sys::CONFIG_DISABLE_OPENGL_DEPTHBUFFER)
			{
				printf("DEPTH TEST ENABLED\n");
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
			}
			else
			{
				printf("NO DEPTH TESTING\n");
				glDisable(GL_DEPTH_TEST);
			}
			

			ww::gfx::window->setKeyRepeatEnabled(false);
			ww::gfx::window->setFramerateLimit(timerResolution);

			ww::input::keyboard::clear();
			if (~CONFIG & ww::sys::CONFIG_DISABLE_OPENAL)
			{
				printf("initialize openAL\n");
				ww::sfx::openal_device = alcOpenDevice(NULL);
				ww::sfx::openal_context = alcCreateContext(ww::sfx::openal_device, NULL);

				ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
				ALfloat listenerPos[] = { 0.f, 0.f, 1.f};
				ALfloat listenerVel[] = { 0.f, 0.f, 0.f};

				alListenerfv(AL_POSITION, listenerPos);
				alListenerfv(AL_VELOCITY, listenerVel);
				alListenerfv(AL_ORIENTATION, listenerOri);

				alcMakeContextCurrent(ww::sfx::openal_context);
			}
			if (initFuncPtr != NULL)
				initFuncPtr();

			if (~CONFIG & sys::CONFIG_CUSTOM_TIMER)
			{
				while(ww::gfx::window->isOpen())
				{
					ww::sys::customTimer_timerBegin();
					if (timerFuncPtr != NULL)
						if (timerFuncPtr() == false)
							break;
					ww::gfx::display();
					ww::sys::customTimer_timerFinish();
				}
			}
			if (deinitFuncPtr != NULL)
				deinitFuncPtr();
			ww::sys::customTimer_unsetup();
			if (~CONFIG & CONFIG_DISABLE_OPENAL)
			{
				alcMakeContextCurrent(NULL);
				alcDestroyContext(ww::sfx::openal_context);
				alcCloseDevice(ww::sfx::openal_device);
			}
			return 0;
		}

		int setup_apple(unsigned int config, int argc, char *argv[])
		{
			CONFIG = config;
#if PLATFORM_IOS
			return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
#elif PLATFORM_OSX
			if (CONFIG & sys::CONFIG_CUSTOM_TIMER)
			{
				init(config);
			}
			else
				return NSApplicationMain(argc, (const char **)argv);
#endif
			return 0;
		}

	void customTimer_unsetup()
		{
#if PLATFORM_WINDOWS
			timeEndPeriod(1);
			WSACleanup();
#endif
		}

		void customTimer_timerBegin()
		{
			sf::Event event;
			while(ww::gfx::window->pollEvent(event))
			{
				if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
					ww::input::keyboard::handleSfEvent(event);
				if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::MouseMoved)
					ww::input::mouse::handleSfEvent(event);
				if (event.type == sf::Event::Closed)
					ww::gfx::window->close();
			}
			ww::gui::handle_messages(true);
			if (CONFIG & CONFIG_XINPUT)
				ww::input::xbox360::handleEvent();
		}
		void customTimer_timerFinish()
		{
			ww::input::keyboard::endOfFrame();
			ww::input::mouse::endOfFrame();
			if (CONFIG & CONFIG_XINPUT)
				ww::input::xbox360::endOfFrame();
		}
	};
};