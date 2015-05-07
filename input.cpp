#include <WadeWork/sys.h>
#include <WadeWork/sys_defines.h>
#include <WadeWork/input.h>
#if PLATFORM_PC
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#if PLATFORM_WINDOWS
#include <xinput.h>
#endif
#endif

namespace ww
{
	namespace gfx
	{
		extern sf::Window *window;
	}
	namespace input
	{
		namespace keyboard
		{
			bool keystateDown[256];
			bool keystatePressed[256];
			bool keystateReleased[256];
			int lastKeyPressed;
			int lastCharPressed;
			int lastKeyPressedGM;				// GM denotes GM-like functionality
			std::string lastCharPressedGM;	//				/\ 
			std::string keyboard_stringGM;	//				/\

			unsigned int sf2vk(sf::Keyboard::Key key);
			sf::Keyboard::Key vk2sf(unsigned int key);
			unsigned int char2key(char c);
			std::string key2string(unsigned int key);

			void handleSfEvent(sf::Event event)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					unsigned int VK = sf2vk(event.key.code);
					keystateDown[VK] = true;
					keystatePressed[VK] = true;

					lastKeyPressed = VK;
					if (VK >= ww::input::key::A && VK <= ww::input::key::Z)
						lastCharPressed = 'A'+(VK-ww::input::key::A);
					if (VK >= ww::input::key::Num0 && VK <= ww::input::key::Num9)
						lastCharPressed = '0'+(VK-ww::input::key::Num0);
					lastKeyPressedGM = lastKeyPressed;
					if (key2string(lastKeyPressed) != "")
						lastCharPressedGM = key2string(lastKeyPressed);
					if (lastKeyPressedGM == key::Backspace)
					{
						if (keyboard_stringGM.length() > 0)
							keyboard_stringGM.erase(keyboard_stringGM.begin()+keyboard_stringGM.size()-1);
					}
					else if (lastCharPressedGM)
					{
						keyboard_stringGM += lastCharPressedGM;
					}
					while(keyboard_stringGM.size() > 1024)
						keyboard_stringGM.erase(keyboard_stringGM.begin());
				}
				else if (event.type == sf::Event::KeyReleased)
				{
					unsigned int VK = sf2vk(event.key.code);
					keystateDown[VK] = false;
					keystateReleased[VK] = true;
				}
			}

			void ios_addKeys(const char *keys, unsigned long length)
			{
				if (keys == 0)
				{
					printf("\t\t\t\t\t---------ENTER WOO---------\n");
					if (length == 13)
					{
						keystatePressed[key::Return] = keystateDown[key::Return] = true;
						lastCharPressedGM = key::Return;
						lastKeyPressedGM = key::Return;
					}
					if (length == 9)
					{
						keystatePressed[key::Backspace] = keystateDown[key::Backspace] = true;
						lastCharPressedGM = key::Backspace;
						lastKeyPressedGM = key::Backspace;
						
						if (keyboard_stringGM.length() > 0)
							keyboard_stringGM.erase(keyboard_stringGM.begin()+keyboard_stringGM.size()-1);
					}
					return;
				}
				for(unsigned long i = 0; i < length; i++)
				{
					printf("\t\tcharacter %c pressed.\n",keys[i]);
					unsigned int charkey = char2key(keys[i]);
					//keystateDown[charkey] = true;
					keystatePressed[charkey] = true;
					if (i == length-1)
					{
						printf("\t\tlast chararacter is %c\n",keys[i]);
						
						lastCharPressed = keys[i];
						lastCharPressedGM = keys[i];
						
						lastKeyPressed = charkey;
						lastKeyPressedGM = charkey;
					}
				}
				keyboard_stringGM += keys;
			}

			std::string getTypedText()
			{
				return keyboard_stringGM;
			}
			void setTypedText(std::string text)
			{
				keyboard_stringGM = text;
			}

			void endOfFrame()
			{
				for(int i=0;i<256;i++)
					keystatePressed[i] = keystateReleased[i] = false;
			}


			void clear()
			{
				for(int i=0;i<256;i++)
					keystateDown[i] = keystatePressed[i] = keystateReleased[i] = false;
			}

			bool isKeyPressed(unsigned int button)
			{
				if (button > 256)
					return false;
				return keystatePressed[button];
			}
			bool isKeyDown(unsigned int button)
			{
				if (button > 256)
					return false;
				return keystateDown[button];
			}
			bool isKeyReleased(unsigned int button)
			{
				if (button > 256)
					return false;
				return keystateReleased[button];
			}
		};

		namespace mouse
		{
#if PLATFORM_PC
			bool mousestateDown[3];
			bool mousestatePressed[3];
			bool mousestateReleased[3];
			int wheelDelta;
			vec2di mousePosition;

			void handleSfEvent(sf::Event event)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					mousePosition.x = event.mouseMove.x;
					mousePosition.y = event.mouseMove.y;
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					mousePosition.x = event.mouseButton.x;
					mousePosition.y = event.mouseButton.y;
					mousestateDown[event.mouseButton.button] = true;
					mousestatePressed[event.mouseButton.button] = true;					
				}
				else if (event.type == sf::Event::MouseButtonReleased)
				{
					mousePosition.x = event.mouseButton.x;
					mousePosition.y = event.mouseButton.y;
					mousestateDown[event.mouseButton.button] = false;
					mousestateReleased[event.mouseButton.button] = true;
				}
				else if (event.type == sf::Event::MouseWheelMoved)
				{
					printf("WHOO! %i\n",event.mouseWheel.delta);
					if (event.mouseWheel.delta < 0)
						wheelDelta = -1;
					if (event.mouseWheel.delta > 0)
						wheelDelta = 1;
				}

			}
			void endOfFrame()
			{
				for(int i=0;i<3;i++)
					mousestatePressed[i] = mousestateReleased[i] = false;
				wheelDelta = 0;
			}

			int wheel()
			{
				return wheelDelta;
			}

			vec2di getPosition()
			{
				if (ww::gfx::window)
					return vec2di(sf::Mouse::getPosition(*ww::gfx::window).x,sf::Mouse::getPosition(*ww::gfx::window).y);
				else
					return vec2di(sf::Mouse::getPosition().x,sf::Mouse::getPosition().y);
			}
			int getX()
			{
				if (ww::gfx::window)
					return sf::Mouse::getPosition(*ww::gfx::window).x;
				else
					return sf::Mouse::getPosition().x;
			}
			int getY()
			{
				if (ww::gfx::window)
					return sf::Mouse::getPosition(*ww::gfx::window).y;
				else
					return sf::Mouse::getPosition().y;
			}
			bool hits(ww::Rectanglei rect)
			{
				int xx = 0;
				int yy = 0;
				if (ww::gfx::window)
				{
					xx = sf::Mouse::getPosition(*ww::gfx::window).x;
					yy = sf::Mouse::getPosition(*ww::gfx::window).y;
				}
				else
				{
					xx = sf::Mouse::getPosition().x;
					yy = sf::Mouse::getPosition().y;
				}
					
				if (xx >= rect.x)
					if (yy >= rect.y)
						if (xx < rect.x + rect.width)
							if (yy < rect.y + rect.height)
								return true;
				return false;
			}

			void clear()
			{
				for(int i=0;i<3;i++)
					mousestateDown[i] = mousestatePressed[i] = mousestateReleased[i] = false;
			}

			bool isButtonPressed(unsigned int button)
			{
				if (button > 2)
					return false;
				return mousestatePressed[button];
			}
			bool isButtonDown(unsigned int button)
			{
				if (button > 2)
					return false;
				return mousestateDown[button];
			}
			bool isButtonReleased(unsigned int button)
			{
				if (button > 2)
					return false;
				return mousestateReleased[button];
			}
#else
			vec2di getPosition()
			{
				return vec2di(0,0);
			}
			int getX()
			{
				return 0;
			}
			int getY()
			{
				return 0;
			}

			void clear()
			{
			}

			bool isButtonPressed(unsigned int button)
			{
				return false;
			}
			bool isButtonDown(unsigned int button)
			{
				return false;
			}
			bool isButtonReleased(unsigned int button)
			{
				return false;
			}
#endif
		};

		namespace touch
		{
#if PLATFORM_IOS
			int maxCount;
			int touchCount;
			std::vector<UITouch *> touches;
			bool swipe[4];
			bool newSwipe[4];
			UIViewController *controller;
			CGSize screenSize;
			
			bool hasDoubleTap;
			CGPoint doubleTapLocation;
			
			bool hasTap;
			CGPoint tapLocation;

			void update()
			{
				swipe[SWIPE_LEFT] = newSwipe[SWIPE_LEFT];
				swipe[SWIPE_UP] = newSwipe[SWIPE_UP];
				swipe[SWIPE_RIGHT] = newSwipe[SWIPE_RIGHT];
				swipe[SWIPE_DOWN] = newSwipe[SWIPE_DOWN];
				newSwipe[SWIPE_LEFT] = false;
				newSwipe[SWIPE_UP] = false;
				newSwipe[SWIPE_RIGHT] = false;
				newSwipe[SWIPE_DOWN] = false;
			}
			void endOfFrame()
			{
				hasTap = false;
				hasDoubleTap = false;
			}
			
			TouchInput_iOS(InputManager *man)
			{
				manager = man;
				/*if (ipad)
					maxcount = 10;
				 else
					maxcount = 5;
				 */
				maxCount = 10;
				clearTouches();
				controller = ((AppDelegate*)[[UIApplication sharedApplication] delegate]).window.rootViewController;
				screenSize = [UIScreen mainScreen].bounds.size;
			}
			void clearTouches()
			{
				touchCount = 0;
				touches.clear();
			}
			bool swiped(int direction)
			{
				if (direction >= 0 && direction <= 3)
					return swipe[direction];
				return false;
			}
			void addDoubleTap(CGPoint location)
			{
				hasDoubleTap = true;
				doubleTapLocation = location;
			}
			bool doubleTapped()
			{
				return hasDoubleTap;
			}
			bool doubleTapped(int x, int y, int x2, int y2)
			{
				if (hasDoubleTap)
				{
					int xx = fixTouchForOrientation(doubleTapLocation).x;
					int yy = fixTouchForOrientation(doubleTapLocation).y;
					if (xx >= x)
						if (xx < x2)
							if (yy >= y)
								if (yy < y2)
									return true;
				}
				return false;
			}
			bool doubleTappedArea(int x, int y, int w, int h)
			{
				if (hasDoubleTap)
				{
					//printf("Is there a tap in (%i,%i) wiwth size %i %i?\n",x,y,w,h);
					int xx = fixTouchForOrientation(doubleTapLocation).x;
					int yy = fixTouchForOrientation(doubleTapLocation).y;
					if (xx >= x)
						if (xx < x+w)
							if (yy >= y)
								if (yy < y+h)
								{
									//printf("\tYES!\n");
									return true;
								}
				}
				return false;
			}
			void addTap(CGPoint location)
			{
				hasTap = true;
				tapLocation = location;
			}
			bool tapped()
			{
				if (hasTap)
					return true;
				return false;
			}
			bool tapped(int x, int y, int x2, int y2)
			{
				if (hasTap)
				{
					int xx = fixTouchForOrientation(tapLocation).x;
					int yy = fixTouchForOrientation(tapLocation).y;
					if (xx >= x)
						if (xx < x2)
							if (yy >= y)
								if (yy < y2)
									return true;
				}
				return false;
			}
			bool tappedArea(int x, int y, int w, int h)
			{
				if (hasTap)
				{
					//printf("Is there a tap in (%i,%i) wiwth size %i %i?\n",x,y,w,h);
					int xx = fixTouchForOrientation(tapLocation).x;
					int yy = fixTouchForOrientation(tapLocation).y;
					if (xx >= x)
						if (xx < x+w)
							if (yy >= y)
								if (yy < y+h)
								{
									//printf("\tYES!\n");
									return true;
								}
				}
				return false;
			}
			bool touched()
			{
				return (touches.size() > 0);
			}
			bool touched(int x, int y, int x2, int y2)
			{
				for(int i=0;i<touches.size();i++)
					if (touches.at(i) != NULL)
					{
						CGPoint p = fixTouchForOrientation([touches.at(i) locationInView:nil]);
						if (p.x >= x)
							if (p.x < x2)
								if (p.y >= y)
									if (p.y < y2)
										return true;
					}
				return false;
			}
			bool touchedArea(int x, int y, int w, int h)
			{
				for(int i=0;i<touches.size();i++)
					if (touches.at(i) != NULL)
					{
						CGPoint p = fixTouchForOrientation([touches.at(i) locationInView:nil]);
						if (p.x >= x)
							if (p.x < x+w)
								if (p.y >= y)
									if (p.y < y+h)
										return true;
					}
				return false;
			}
			CGPoint fixTouchForOrientation(CGPoint p)
			{
				CGPoint retval = p;
				switch(controller.interfaceOrientation)
				{
					case UIInterfaceOrientationPortrait:
						//NSLog(@"%f %f",retval.x,retval.y);
						return retval;
						break;
					case UIInterfaceOrientationPortraitUpsideDown:
						retval.x = screenSize.height-p.x;
						retval.y = screenSize.width-p.y;
						//NSLog(@"%f %f",retval.x,retval.y);
						return retval;
						break;
					case UIInterfaceOrientationLandscapeLeft:
						retval.x = screenSize.height-p.y;
						retval.y = p.x;
						//NSLog(@"%f %f",retval.x,retval.y);
						return retval;
						break;
					case UIInterfaceOrientationLandscapeRight:
						retval.x = p.y;
						retval.y = screenSize.width-p.x;
						//NSLog(@"%f %f",retval.x,retval.y);
						return retval;
						break;
					default:
						return retval;
						break;
				}
				return p;
			}

			void addTouch(UITouch *touch)
			{
				touches.push_back(touch);
				//printf("touchCount: %i\n",(int)touches.size());
			}
			
			void addSwipe(int direction)
			{
				printf("direction = %i\n",direction);
				if (direction >= 0 && direction <= 3)
					newSwipe[direction] = true;
			}
			
			void removeTouch(UITouch *touch)
			{
				for(int i=0;i<touches.size();i++)
				{
					if (touches.at(i) == touch)
					{
						touches.erase(touches.begin()+i);
						i--;
					}
				}
				//printf("touchCount: %i\n",(int)touches.size());
			}
			
			unsigned int rawtouchCount()
			{
				return touches.size();
			}
			ww::vec2di rawtouchLocation(unsigned int index)
			{
				if (index < touches.size())
					return vec2di(fixTouchForOrientation([touches.at(index) locationInView:nil]).x,fixTouchForOrientation([touches.at(index) locationInView:nil]).y);
				else
					return vec2di(-1000,-1000);
			}
#else
			void clear()
			{
			}
			bool swiped(int direction)
			{
				return false;
			}
			bool touched()
			{
				return false;
			}
			bool touched(int x, int y, int x2, int y2)
			{
				return false;
			}
			bool touchedArea(int x, int y, int w, int h)
			{
				return false;
			}
			
			bool tapped()
			{
				return false;
			}
			bool tapped(int x, int y, int x2, int y2)
			{
				return false;
			}
			bool tappedArea(int x, int y, int w, int h)
			{
				return false;
			}
			
			bool doubleTapped()
			{
				return false;
			}
			bool doubleTapped(int x, int y, int x2, int y2)
			{
				return false;
			}
			bool doubleTappedArea(int x, int y, int w, int h)
			{
				return false;
			}
			
			unsigned int rawtouchCount()
			{
				return 0;
			}
			vec2di rawtouchLocation(unsigned int index)
			{
				return vec2di(0,0);
			}
#endif
		}

		namespace accelerometer
		{
#if PLATFORM_IOS
			vec3df accel;

			void handleEvent()
			{
				CMMotionManager *m = [(AppDelegate*)[[UIApplication sharedApplication] delegate] motionManager];
				accel.x = [[m accelerometerData] acceleration].x;
				accel.y = [[m accelerometerData] acceleration].y;
				accel.z = [[m accelerometerData] acceleration].z;
			}
			vec3df get()
			{
				return accel;
			}
			extern float getX()
			{
				return accel.x;
			}
			extern float getY()
			{
				return accel.y;
			}
			extern float getZ()
			{
				return accel.z;
			}
#else
			extern vec3df get()
			{
				return vec3df(0.f,0.f,0.f);
			}
			extern float getX()
			{
				return 0.f;
			}
			extern float getY()
			{
				return 0.f;
			}
			extern float getZ()
			{
				return 0.f;
			}
#endif
		};

		namespace iCade
		{
			extern bool isButtonPressed(unsigned int button);
			extern bool isButtonDown(unsigned int button);
			extern bool isButtonReleased(unsigned int button);
		};

		namespace xbox360
		{
#if !PLATFORM_WINDOWS
			extern bool isConnected(int controller)
			{
				return false;
			}
			extern vec2df getLeftThumb(int controller)
			{
				return vec2df(0.f,0.f);
			}
			extern vec2df getRightThumb(int controller)
			{
				return vec2df(0.f,0.f);
			}
			extern float getLeftTrigger(int controller);
			{
				return 0.f;
			}
			extern float getRightTrigger(int controller)
			{
				return 0.f;
			}
			
			extern bool isButtonPressed(int controller, int id)
			{
				return false;
			}
			extern bool isButtonDown(int controller, int id)
			{
				return false;
			}
			extern bool isButtonReleased(int controller, int id)
			{
				return false;
			}
#else
			const unsigned int MAX_CONTROLLERS = 4;
			DWORD dwPacketNumber[MAX_CONTROLLERS];
			bool bConnected[MAX_CONTROLLERS];
			
			float fThumbLX[MAX_CONTROLLERS];
			float fThumbLY[MAX_CONTROLLERS];
			float fThumbRX[MAX_CONTROLLERS];
			float fThumbRY[MAX_CONTROLLERS];
			float fLeftTrigger[MAX_CONTROLLERS];
			float fRightTrigger[MAX_CONTROLLERS];
			WORD wButtons[MAX_CONTROLLERS];
			
			float fThumbLXPrevious[MAX_CONTROLLERS];
			float fThumbLYPrevious[MAX_CONTROLLERS];
			float fThumbRXPrevious[MAX_CONTROLLERS];
			float fThumbRYPrevious[MAX_CONTROLLERS];
			float fLeftTriggerPrevious[MAX_CONTROLLERS];
			float fRightTriggerPrevious[MAX_CONTROLLERS];
			WORD wButtonsPrevious[MAX_CONTROLLERS];

			void clear()
			{
				for(int i=0;i<MAX_CONTROLLERS;i++)
				{
					fThumbLXPrevious[i] = fThumbLXPrevious[i] = 0;
					fThumbLYPrevious[i] = fThumbLY[i] = 0;
					fThumbRXPrevious[i] = fThumbRX[i] = 0;
					fThumbRYPrevious[i] = fThumbRY[i] = 0;
					fLeftTriggerPrevious[i] = fLeftTrigger[i] = 0;
					fRightTriggerPrevious[i] = fRightTrigger[i] = 0;
					wButtonsPrevious[i] = wButtons[i] = 0;
				}
			}
			void handleEvent()
			{
				DWORD dwResult;
				XINPUT_STATE state;
				for(int id=0;id<MAX_CONTROLLERS;id++)
				{
					ZeroMemory(&state,sizeof(XINPUT_STATE));
					dwResult = XInputGetState(id,&state);
					if (dwResult == ERROR_SUCCESS)
					{
						bConnected[id] = true;
						//printf("XBOX -- Controller %i connected\n",id);
						if (state.dwPacketNumber != dwPacketNumber[id])
						{
							fLeftTrigger[id] = (float) (state.Gamepad.bLeftTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD < 0 ? 0 : state.Gamepad.bLeftTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (float) (255-XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
							fRightTrigger[id] = (float) (state.Gamepad.bRightTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD < 0 ? 0 : state.Gamepad.bRightTrigger-XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (float) (255-XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
							bool neg = false;
							SHORT thumbRawX, thumbRawY;
							SHORT fixedX, fixedY, deadzone;
							fixedX = -1000;
							fixedY = -1000;
							thumbRawX = state.Gamepad.sThumbLX;
							thumbRawY = state.Gamepad.sThumbLY;
							//printf("XBOX -- Controller %i raw left thumb values: %i %i\n",id,state.Gamepad.sThumbLX,state.Gamepad.sThumbLY);
							deadzone = 32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
							if (thumbRawX < 0)
								if (thumbRawX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
									fixedX = 0;
							if (thumbRawX > 0)
								if (thumbRawX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
									fixedX = 0;
							if (thumbRawX < 0 && fixedX == -1000)
								fixedX = thumbRawX+XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
							if (thumbRawX > 0 && fixedX == -1000)
								fixedX = thumbRawX-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

							if (thumbRawY < 0)
								if (thumbRawY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
									fixedY = 0;
							if (thumbRawY > 0)
								if (thumbRawY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
									fixedY = 0;
							if (thumbRawY < 0 && fixedY == -1000)
								fixedY = thumbRawY+XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
							if (thumbRawY > 0 && fixedY == -1000)
								fixedY = thumbRawY-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
							if (fixedX != -1000)
								fThumbLX[id] = abs((float)(fixedX)) / (float)(deadzone) * (thumbRawX < 0 ? -1 : 1);
							if (fixedY != -1000)
								fThumbLY[id] = abs((float)(fixedY)) / (float)(deadzone) * (thumbRawY < 0 ? 1 : -1);

							//printf("XBOX -- Controller %i fixed left thumb values: %f %f\n",id,fThumbLX[id],fThumbLY[id]);


							fixedX = -1000;
							fixedY = -1000;
							thumbRawX = state.Gamepad.sThumbRX;
							thumbRawY = state.Gamepad.sThumbRY;
							deadzone = 32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
							if (thumbRawX < 0)
								if (thumbRawX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
									fixedX = 0;
							if (thumbRawX > 0)
								if (thumbRawX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
									fixedX = 0;
							if (thumbRawX < 0 && fixedX == -1000)
								fixedX = thumbRawX+XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
							if (thumbRawX > 0 && fixedX == -1000)
								fixedX = thumbRawX-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

							if (thumbRawY < 0)
								if (thumbRawY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
									fixedY = 0;
							if (thumbRawY > 0)
								if (thumbRawY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
									fixedY = 0;
							if (thumbRawY < 0 && fixedY == -1000)
								fixedY = thumbRawY+XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
							if (thumbRawY > 0 && fixedY == -1000)
								fixedY = thumbRawY-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
							if (fixedX != -1000)
								fThumbRX[id] = abs((float)(fixedX)) / (float)(deadzone) * (thumbRawX < 0 ? -1 : 1);
							if (fixedY != -1000)
								fThumbRY[id] = abs((float)(fixedY)) / (float)(deadzone) * (thumbRawY < 0 ? 1 : -1);
							wButtons[id]  = state.Gamepad.wButtons;
						}
						dwPacketNumber[id] = state.dwPacketNumber;
					}
					else
						bConnected[id] = false;
				}
			}
			void endOfFrame()
			{
				for(int i=0;i<MAX_CONTROLLERS;i++)
				{
					fThumbLXPrevious[i] = fThumbLXPrevious[i];
					fThumbLYPrevious[i] = fThumbLY[i];
					fThumbRXPrevious[i] = fThumbRX[i];
					fThumbRYPrevious[i] = fThumbRY[i];
					fLeftTriggerPrevious[i] = fLeftTrigger[i];
					fRightTriggerPrevious[i] = fRightTrigger[i];
					wButtonsPrevious[i] = wButtons[i];
				}
			}
			bool isConnected(int controller)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return false;
				return bConnected[controller];
			}
			vec2df getLeftThumb(int controller)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return vec2df(0,0);
				return vec2df(fThumbLX[controller],fThumbLY[controller]);
			}
			vec2df getRightThumb(int controller)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return vec2df(0,0);
				return vec2df(fThumbRX[controller],fThumbRY[controller]);
			}
			float getLeftTrigger(int controller)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return 0.f;
				return fLeftTrigger[controller];
			}
			float getRightTrigger(int controller)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return 0.f;
				return fRightTrigger[controller];
			}
			bool isButtonPressed(int controller, int id)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return false;
				return (wButtons[controller] & id) && !(wButtonsPrevious[controller] & id);
			}
			bool isButtonReleased(int controller, int id)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return false;
				return !(wButtons[controller] & id) && (wButtonsPrevious[controller] & id);
			}
			bool isButtonDown(int controller, int id)
			{
				if (controller < 0 || controller >= MAX_CONTROLLERS)
					return false;
				return (wButtons[controller] & id) > 0;
			}
#endif
		};
	};
};

namespace ww
{
	namespace input
	{
		namespace keyboard
		{
			unsigned int sf2vk(sf::Keyboard::Key _key)
			{
				switch(_key)
				{
		#if PLATFORM_WINDOWS
				case sf::Keyboard::Key::Back:
		#else
				case sf::Keyboard::Key::BackSpace:
		#endif
					return ww::input::key::Backspace;
					break;
				case sf::Keyboard::Key::Tab:
					return ww::input::key::Tab;
					break;
				case sf::Keyboard::Key::Return:
					return ww::input::key::Return;
					break;
				//return ww::input::key::Shift; 0x10:
				//return ww::input::key::Control; 0x11:
				//return ww::input::key::Alt; 0x12:
				case sf::Keyboard::Key::Pause:
					return ww::input::key::Pause;
					break;
				//return ww::input::key::Capslock; 0x14:
				case sf::Keyboard::Key::Escape:
					return ww::input::key::Escape;
					break;
				case sf::Keyboard::Key::Space:
					return ww::input::key::Space;
					break;
				case sf::Keyboard::Key::PageDown:
					return ww::input::key::PageUp;
					break;
				case sf::Keyboard::Key::PageUp:
					return ww::input::key::PageDown;
					break;
				case sf::Keyboard::Key::End:
					return ww::input::key::End;
					break;
				case sf::Keyboard::Key::Home:
					return ww::input::key::Home;
					break;
				case sf::Keyboard::Key::Left:
					return ww::input::key::Left;
					break;
				case sf::Keyboard::Key::Up:
					return ww::input::key::Up;
					break;
				case sf::Keyboard::Key::Right:
					return ww::input::key::Right;
					break;
				case sf::Keyboard::Key::Down:
					return ww::input::key::Down;
					break;
				//return ww::input::key::Select; 0x29:
				//return ww::input::key::Print; 0x2A:
				//return ww::input::key::Execute; 0x2B:
				//return ww::input::key::Snapshot; 0x2C:
				case sf::Keyboard::Key::Insert:
					return ww::input::key::Insert;
					break;
				case sf::Keyboard::Key::Delete:
					return ww::input::key::Delete;
					break;
				//return ww::input::key::Help; 0x2F:
				case sf::Keyboard::Key::Num0:
					return ww::input::key::Num0;
					break;
				case sf::Keyboard::Key::Num1:
					return ww::input::key::Num1;
					break;
				case sf::Keyboard::Key::Num2:
					return ww::input::key::Num2;
					break;
				case sf::Keyboard::Key::Num3:
					return ww::input::key::Num3;
					break;
				case sf::Keyboard::Key::Num4:
					return ww::input::key::Num4;
					break;
				case sf::Keyboard::Key::Num5:
					return ww::input::key::Num5;
					break;
				case sf::Keyboard::Key::Num6:
					return ww::input::key::Num6;
					break;
				case sf::Keyboard::Key::Num7:
					return ww::input::key::Num7;
					break;
				case sf::Keyboard::Key::Num8:
					return ww::input::key::Num8;
					break;
				case sf::Keyboard::Key::Num9:
					return ww::input::key::Num9;
					break;
				case sf::Keyboard::Key::A:
					return ww::input::key::A;
					break;
				case sf::Keyboard::Key::B:
					return ww::input::key::B;
					break;
				case sf::Keyboard::Key::C:
					return ww::input::key::C;
					break;
				case sf::Keyboard::Key::D:
					return ww::input::key::D;
					break;
				case sf::Keyboard::Key::E:
					return ww::input::key::E;
					break;
				case sf::Keyboard::Key::F:
					return ww::input::key::F;
					break;
				case sf::Keyboard::Key::G:
					return ww::input::key::G;
					break;
				case sf::Keyboard::Key::H:
					return ww::input::key::H;
					break;
				case sf::Keyboard::Key::I:
					return ww::input::key::I;
					break;
				case sf::Keyboard::Key::J:
					return ww::input::key::J;
					break;
				case sf::Keyboard::Key::K:
					return ww::input::key::K;
					break;
				case sf::Keyboard::Key::L:
					return ww::input::key::L;
					break;
				case sf::Keyboard::Key::M:
					return ww::input::key::M;
					break;
				case sf::Keyboard::Key::N:
					return ww::input::key::N;
					break;
				case sf::Keyboard::Key::O:
					return ww::input::key::O;
					break;
				case sf::Keyboard::Key::P:
					return ww::input::key::P;
					break;
				case sf::Keyboard::Key::Q:
					return ww::input::key::Q;
					break;
				case sf::Keyboard::Key::R:
					return ww::input::key::R;
					break;
				case sf::Keyboard::Key::S:
					return ww::input::key::S;
					break;
				case sf::Keyboard::Key::T:
					return ww::input::key::T;
					break;
				case sf::Keyboard::Key::U:
					return ww::input::key::U;
					break;
				case sf::Keyboard::Key::V:
					return ww::input::key::V;
					break;
				case sf::Keyboard::Key::W:
					return ww::input::key::W;
					break;
				case sf::Keyboard::Key::X:
					return ww::input::key::X;
					break;
				case sf::Keyboard::Key::Y:
					return ww::input::key::Y;
					break;
				case sf::Keyboard::Key::Z:
					return ww::input::key::Z;
					break;
				case sf::Keyboard::Key::LSystem:
					return ww::input::key::LWindows;
					break;
				case sf::Keyboard::Key::RSystem:
					return ww::input::key::RWindows;
					break;
				//return ww::input::key::Applications; 0x5D:
				//return ww::input::key::Sleep; 0x5F:
				case sf::Keyboard::Key::Numpad0:
					return ww::input::key::Numpad0;
					break;
				case sf::Keyboard::Key::Numpad1:
					return ww::input::key::Numpad1;
					break;
				case sf::Keyboard::Key::Numpad2:
					return ww::input::key::Numpad2;
					break;
				case sf::Keyboard::Key::Numpad3:
					return ww::input::key::Numpad3;
					break;
				case sf::Keyboard::Key::Numpad4:
					return ww::input::key::Numpad4;
					break;
				case sf::Keyboard::Key::Numpad5:
					return ww::input::key::Numpad5;
					break;
				case sf::Keyboard::Key::Numpad6:
					return ww::input::key::Numpad6;
					break;
				case sf::Keyboard::Key::Numpad7:
					return ww::input::key::Numpad7;
					break;
				case sf::Keyboard::Key::Numpad8:
					return ww::input::key::Numpad8;
					break;
				case sf::Keyboard::Key::Numpad9:
					return ww::input::key::Numpad9;
					break;
				case sf::Keyboard::Key::Multiply:
					return ww::input::key::Multiply;
					break;
				case sf::Keyboard::Key::Add:
					return ww::input::key::Add;
					break;
				//return ww::input::key::Separator; 0x6C:
				case sf::Keyboard::Key::Subtract:
					return ww::input::key::Subtract;
					break;
				//return ww::input::key::Decimal; 0x6E:
				//	case sf::Keyboard::Key::Peri:
				//	break;
				case sf::Keyboard::Key::Divide:
					return ww::input::key::Divide;
					break;
				case sf::Keyboard::Key::F1:
					return ww::input::key::F1;	
					break;
				case sf::Keyboard::Key::F2:
					return ww::input::key::F2;
					break;
				case sf::Keyboard::Key::F3:
					return ww::input::key::F3;
					break;
				case sf::Keyboard::Key::F4:
					return ww::input::key::F4;
					break;
				case sf::Keyboard::Key::F5:
					return ww::input::key::F5;
					break;
				case sf::Keyboard::Key::F6:
					return ww::input::key::F6;	
					break;
				case sf::Keyboard::Key::F7:
					return ww::input::key::F7;
					break;
				case sf::Keyboard::Key::F8:
					return ww::input::key::F8;
					break;
				case sf::Keyboard::Key::F9:
					return ww::input::key::F9;
					break;
				case sf::Keyboard::Key::F10:
					return ww::input::key::F10;
					break;
				case sf::Keyboard::Key::F11:
					return ww::input::key::F11;
					break;
				case sf::Keyboard::Key::F12:
					return ww::input::key::F12;
					break;
				case sf::Keyboard::Key::F13:
					return ww::input::key::F13;
					break;
				case sf::Keyboard::Key::F14:
					return ww::input::key::F14;
					break;
				case sf::Keyboard::Key::F15:
					return ww::input::key::F15;
					break;
				//return ww::input::key::F16; 0x7F:
				//return ww::input::key::F17; 0x80:
				//return ww::input::key::F18; 0x81:
				//return ww::input::key::F19; 0x82:
				//return ww::input::key::F20; 0x83:
				//return ww::input::key::F21; 0x84:
				//return ww::input::key::F22; 0x85:
				//return ww::input::key::F23; 0x86:
				//return ww::input::key::F24; 0x87:
				case sf::Keyboard::Key::LShift:
					return ww::input::key::LShift;
					break;
				case sf::Keyboard::Key::RShift:
					return ww::input::key::RShift;
					break;
				case sf::Keyboard::Key::LControl:
					return ww::input::key::LControl;
					break;
				case sf::Keyboard::Key::RControl:
					return ww::input::key::RControl;
					break;
				case sf::Keyboard::Key::SemiColon:
					return ww::input::key::Semicolon;	
					break;
				//return ww::input::key::Plus;
				case sf::Keyboard::Key::Comma:
					return ww::input::key::Comma;
					break;
				//return ww::input::key::Minus; 0xBD:
				case sf::Keyboard::Key::Period:
					return ww::input::key::Period;
					break;
				case sf::Keyboard::Key::BackSlash:
					return ww::input::key::Backslash;
					break;
				case sf::Keyboard::Key::Tilde:
					return ww::input::key::Tilde;
					break;
				case sf::Keyboard::Key::LBracket:
					return ww::input::key::LBracket;
					break;
				case sf::Keyboard::Key::Slash:
					return ww::input::key::Slash;
					break;
				case sf::Keyboard::Key::RBracket:
					return ww::input::key::RBracket;
					break;
				case sf::Keyboard::Key::Quote:
					return ww::input::key::Quote;
					break;
				default:
					return 0;
					break;
				}
			}

			sf::Keyboard::Key vk2sf(unsigned int vk)
			{
				switch(vk)
				{
				case ww::input::key::Backspace:
		#if PLATFORM_WINDOWS
					return sf::Keyboard::Key::Back;
		#else
					return sf::Keyboard::Key::BackSpace;
		#endif
					break;
				case ww::input::key::Tab:
					return sf::Keyboard::Key::Tab;
					break;
				case ww::input::key::Return:
					return sf::Keyboard::Key::Return;
					break;
				//case ww::input::key::Shift: 0x10;
				//case ww::input::key::Control: 0x11;
				//case ww::input::key::Alt: 0x12;
				case ww::input::key::Pause:
					return sf::Keyboard::Key::Pause;
					break;
				//case ww::input::key::Capslock: 0x14;
				case ww::input::key::Escape:
					return sf::Keyboard::Key::Escape;
					break;
				case ww::input::key::Space:
					return sf::Keyboard::Key::Space;
					break;
				case ww::input::key::PageUp:
					return sf::Keyboard::Key::PageDown;
					break;
				case ww::input::key::PageDown:
					return sf::Keyboard::Key::PageUp;
					break;
				case ww::input::key::End:
					return sf::Keyboard::Key::End;
					break;
				case ww::input::key::Home:
					return sf::Keyboard::Key::Home;
					break;
				case ww::input::key::Left:
					return sf::Keyboard::Key::Left;
					break;
				case ww::input::key::Up:
					return sf::Keyboard::Key::Up;
					break;
				case ww::input::key::Right:
					return sf::Keyboard::Key::Right;
					break;
				case ww::input::key::Down:
					return sf::Keyboard::Key::Down;
					break;
				//case ww::input::key::Select: 0x29;
				//case ww::input::key::Print: 0x2A;
				//case ww::input::key::Execute: 0x2B;
				//case ww::input::key::Snapshot: 0x2C;
				case ww::input::key::Insert:
					return sf::Keyboard::Key::Insert;
					break;
				case ww::input::key::Delete:
					return sf::Keyboard::Key::Delete;
				//case ww::input::key::Help: 0x2F;
				case ww::input::key::Num0:
					return sf::Keyboard::Key::Num0;
					break;
				case ww::input::key::Num1:
					return sf::Keyboard::Key::Num1;
					break;
				case ww::input::key::Num2:
					return sf::Keyboard::Key::Num2;
					break;
				case ww::input::key::Num3:
					return sf::Keyboard::Key::Num3;
					break;
				case ww::input::key::Num4:
					return sf::Keyboard::Key::Num4;
					break;
				case ww::input::key::Num5:
					return sf::Keyboard::Key::Num5;
					break;
				case ww::input::key::Num6:
					return sf::Keyboard::Key::Num6;
					break;
				case ww::input::key::Num7:
					return sf::Keyboard::Key::Num7;
					break;
				case ww::input::key::Num8:
					return sf::Keyboard::Key::Num8;
					break;
				case ww::input::key::Num9:
					return sf::Keyboard::Key::Num9;
					break;
				case ww::input::key::A:
					return sf::Keyboard::Key::A;
					break;
				case ww::input::key::B:
					return sf::Keyboard::Key::B;
					break;
				case ww::input::key::C:
					return sf::Keyboard::Key::C;
					break;
				case ww::input::key::D:
					return sf::Keyboard::Key::D;
					break;
				case ww::input::key::E:
					return sf::Keyboard::Key::E;
					break;
				case ww::input::key::F:
					return sf::Keyboard::Key::F;
					break;
				case ww::input::key::G:
					return sf::Keyboard::Key::G;
					break;
				case ww::input::key::H:
					return sf::Keyboard::Key::H;
					break;
				case ww::input::key::I:
					return sf::Keyboard::Key::I;
					break;
				case ww::input::key::J:
					return sf::Keyboard::Key::J;
					break;
				case ww::input::key::K:
					return sf::Keyboard::Key::K;
					break;
				case ww::input::key::L:
					return sf::Keyboard::Key::L;
					break;
				case ww::input::key::M:
					return sf::Keyboard::Key::M;
					break;
				case ww::input::key::N:
					return sf::Keyboard::Key::N;
					break;
				case ww::input::key::O:
					return sf::Keyboard::Key::O;
					break;
				case ww::input::key::P:
					return sf::Keyboard::Key::P;
					break;
				case ww::input::key::Q:
					return sf::Keyboard::Key::Q;
					break;
				case ww::input::key::R:
					return sf::Keyboard::Key::R;
					break;
				case ww::input::key::S:
					return sf::Keyboard::Key::S;
					break;
				case ww::input::key::T:
					return sf::Keyboard::Key::T;
					break;
				case ww::input::key::U:
					return sf::Keyboard::Key::U;
					break;
				case ww::input::key::V:
					return sf::Keyboard::Key::V;
					break;
				case ww::input::key::W:
					return sf::Keyboard::Key::W;
					break;
				case ww::input::key::X:
					return sf::Keyboard::Key::X;
					break;
				case ww::input::key::Y:
					return sf::Keyboard::Key::Y;
					break;
				case ww::input::key::Z:
					return sf::Keyboard::Key::Z;
					break;
				case ww::input::key::LWindows:
					return sf::Keyboard::Key::LSystem;
					break;
				case ww::input::key::RWindows:
					return sf::Keyboard::Key::RSystem;
					break;
				//case ww::input::key::Applications: 0x5D;
				//case ww::input::key::Sleep: 0x5F;
				case ww::input::key::Numpad0:
					return sf::Keyboard::Key::Numpad0;
					break;
				case ww::input::key::Numpad1:
					return sf::Keyboard::Key::Numpad1;
					break;
				case ww::input::key::Numpad2:
					return sf::Keyboard::Key::Numpad2;
					break;
				case ww::input::key::Numpad3:
					return sf::Keyboard::Key::Numpad3;
					break;
				case ww::input::key::Numpad4:
					return sf::Keyboard::Key::Numpad4;
					break;
				case ww::input::key::Numpad5:
					return sf::Keyboard::Key::Numpad5;
					break;
				case ww::input::key::Numpad6:
					return sf::Keyboard::Key::Numpad6;
					break;
				case ww::input::key::Numpad7:
					return sf::Keyboard::Key::Numpad7;
					break;
				case ww::input::key::Numpad8:
					return sf::Keyboard::Key::Numpad8;
					break;
				case ww::input::key::Numpad9:
					return sf::Keyboard::Key::Numpad9;
					break;
				case ww::input::key::Multiply:
					return sf::Keyboard::Key::Multiply;
					break;
				case ww::input::key::Add:
					return sf::Keyboard::Key::Add;
					break;
				//case ww::input::key::Separator: 0x6C;
				case ww::input::key::Subtract:
					return sf::Keyboard::Key::Subtract;
					break;
				//case ww::input::key::Decimal: 0x6E;
				//	return sf::Keyboard::Key::Peri;
				//	break;
				case ww::input::key::Divide:
					return sf::Keyboard::Key::Divide;
					break;
				case ww::input::key::F1:
					return sf::Keyboard::Key::F1;
					break;
				case ww::input::key::F2:
					return sf::Keyboard::Key::F2;
					break;
				case ww::input::key::F3:
					return sf::Keyboard::Key::F3;
					break;
				case ww::input::key::F4:
					return sf::Keyboard::Key::F4;
					break;
				case ww::input::key::F5:
					return sf::Keyboard::Key::F5;
					break;
				case ww::input::key::F6:
					return sf::Keyboard::Key::F6;
					break;
				case ww::input::key::F7:
					return sf::Keyboard::Key::F7;
					break;
				case ww::input::key::F8:
					return sf::Keyboard::Key::F8;
					break;
				case ww::input::key::F9:
					return sf::Keyboard::Key::F9;
					break;
				case ww::input::key::F10:
					return sf::Keyboard::Key::F10;
					break;
				case ww::input::key::F11:
					return sf::Keyboard::Key::F11;
					break;
				case ww::input::key::F12:
					return sf::Keyboard::Key::F12;
					break;
				case ww::input::key::F13:
					return sf::Keyboard::Key::F13;
					break;
				case ww::input::key::F14:
					return sf::Keyboard::Key::F14;
					break;
				case ww::input::key::F15:
					return sf::Keyboard::Key::F15;
					break;
				//case ww::input::key::F16: 0x7F;
				//case ww::input::key::F17: 0x80;
				//case ww::input::key::F18: 0x81;
				//case ww::input::key::F19: 0x82;
				//case ww::input::key::F20: 0x83;
				//case ww::input::key::F21: 0x84;
				//case ww::input::key::F22: 0x85;
				//case ww::input::key::F23: 0x86;
				//case ww::input::key::F24: 0x87;
				case ww::input::key::LShift:
					return sf::Keyboard::Key::LShift;
					break;
				case ww::input::key::RShift:
					return sf::Keyboard::Key::RShift;
					break;
				case ww::input::key::LControl:
					return sf::Keyboard::Key::LControl;
					break;
				case ww::input::key::RControl:
					return sf::Keyboard::Key::RControl;
					break;
				case ww::input::key::Semicolon:
					return sf::Keyboard::Key::SemiColon;
					break;
				//case ww::input::key::Plus:
				case ww::input::key::Comma:
					return sf::Keyboard::Key::Comma;
					break;
				//case ww::input::key::Minus: 0xBD;
				case ww::input::key::Period:
					return sf::Keyboard::Key::Period;
					break;
				case ww::input::key::Backslash:
					return sf::Keyboard::Key::BackSlash;
					break;
				case ww::input::key::Tilde:
					return sf::Keyboard::Key::Tilde;
					break;
				case ww::input::key::LBracket:
					return sf::Keyboard::Key::LBracket;
					break;
				case ww::input::key::Slash:
					return sf::Keyboard::Key::Slash;
					break;
				case ww::input::key::RBracket:
					return sf::Keyboard::Key::RBracket;
					break;
				case ww::input::key::Quote:
					return sf::Keyboard::Key::Quote;
					break;
				default:
					return sf::Keyboard::KeyCount;
					break;
				}
			}

			unsigned int char2key(char c)
			{
				switch(c)
				{
					case ' ':
						return ww::input::key::Space;
						break;
					case '0':
						return ww::input::key::Num0;
						break;
					case '1':
						return ww::input::key::Num1;
						break;
					case '2':
						return ww::input::key::Num2;
						break;
					case '3':
						return ww::input::key::Num3;
						break;
					case '4':
						return ww::input::key::Num4;
						break;
					case '5':
						return ww::input::key::Num5;
						break;
					case '6':
						return ww::input::key::Num6;
						break;
					case '7':
						return ww::input::key::Num7;
						break;
					case '8':
						return ww::input::key::Num8;
						break;
					case '9':
						return ww::input::key::Num9;
						break;
					case 'A': case 'a':
						return ww::input::key::A;
						break;
					case 'B': case 'b':
						return ww::input::key::B;
						break;
					case 'C': case 'c':
						return ww::input::key::C;
						break;
					case 'D': case 'd':
						return ww::input::key::D;
						break;
					case 'E': case 'e':
						return ww::input::key::E;
						break;
					case 'F': case 'f':
						return ww::input::key::F;
						break;
					case 'G': case 'g':
						return ww::input::key::G;
						break;
					case 'H': case 'h':
						return ww::input::key::H;
						break;
					case 'I': case 'i':
						return ww::input::key::I;
						break;
					case 'J': case 'j':
						return ww::input::key::J;
						break;
					case 'K': case 'k':
						return ww::input::key::K;
						break;
					case 'L': case 'l':
						return ww::input::key::L;
						break;
					case 'M': case 'm':
						return ww::input::key::M;
						break;
					case 'N': case 'n':
						return ww::input::key::N;
						break;
					case 'O': case 'o':
						return ww::input::key::O;
						break;
					case 'P': case 'p':
						return ww::input::key::P;
						break;
					case 'Q': case 'q':
						return ww::input::key::Q;
						break;
					case 'R': case 'r':
						return ww::input::key::R;
						break;
					case 'S': case 's':
						return ww::input::key::S;
						break;
					case 'T': case 't':
						return ww::input::key::T;
						break;
					case 'U': case 'u':
						return ww::input::key::U;
						break;
					case 'V': case 'v':
						return ww::input::key::V;
						break;
					case 'W': case 'w':
						return ww::input::key::W;
						break;
					case 'X': case 'x':
						return ww::input::key::X;
						break;
					case 'Y': case 'y':
						return ww::input::key::Y;
						break;
					case 'Z': case 'z':
						return ww::input::key::Z;
						break;
					/*case sf::Keyboard::Key::Numpad0:
						return ww::input::key::Numpad0;
						break;
					case sf::Keyboard::Key::Numpad1:
						return ww::input::key::Numpad1;
						break;
					case sf::Keyboard::Key::Numpad2:
						return ww::input::key::Numpad2;
						break;
					case sf::Keyboard::Key::Numpad3:
						return ww::input::key::Numpad3;
						break;
					case sf::Keyboard::Key::Numpad4:
						return ww::input::key::Numpad4;
						break;
					case sf::Keyboard::Key::Numpad5:
						return ww::input::key::Numpad5;
						break;
					case sf::Keyboard::Key::Numpad6:
						return ww::input::key::Numpad6;
						break;
					case sf::Keyboard::Key::Numpad7:
						return ww::input::key::Numpad7;
						break;
					case sf::Keyboard::Key::Numpad8:
						return ww::input::key::Numpad8;
						break;
					case sf::Keyboard::Key::Numpad9:
						return ww::input::key::Numpad9;
						break;*/
					case '*':
						return ww::input::key::Multiply;
						break;
					case '+':
						return ww::input::key::Add;
						break;
						//return ww::input::key::Separator; 0x6C:
					case '-':
						return ww::input::key::Subtract;
						break;
						//return ww::input::key::Decimal; 0x6E:
						//	case sf::Keyboard::Key::Peri:
						//	break;
					case ';':
						return ww::input::key::Semicolon;
						break;
					case ',':
						return ww::input::key::Comma;
						break;
					case '.':
						return ww::input::key::Period;
						break;
					case '\\':
						return ww::input::key::Backslash;
						break;
					case '~':
						return ww::input::key::Tilde;
						break;
					case '[':
						return ww::input::key::LBracket;
						break;
					case '/':
						return ww::input::key::Slash;
						break;
					case ']':
						return ww::input::key::RBracket;
						break;
					case '\'':
						return ww::input::key::Quote;
						break;
					default:
						return -1;
						break;
				}
			}

			std::string key2string(unsigned int _key)
			{
				std::string s = "";
				if (_key >= ww::input::key::A && _key <= ww::input::key::Z)
					s.push_back('A'+(_key-ww::input::key::A));
				if (_key == ww::input::key::Space)
					s.push_back(' ');
				if (_key >= ww::input::key::Num0 && _key <= ww::input::key::Num9)
					s.push_back('0'+(_key-ww::input::key::Num0));
				if (_key == ww::input::key::Period)
					s.push_back('.');
				if (_key == ww::input::key::Minus)
					s.push_back('-');
				return s;
			}
		};
	};
};