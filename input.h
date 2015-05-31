#ifndef __WadeWork__input_h__
#define __WadeWork__input_h__
#include <WadeWork/types.h>

namespace ww
{
	namespace input
	{
		std::string getName(unsigned int inputID);

		namespace keyboard
		{
			const unsigned int NONE = 0;
			const unsigned int UPPERCASE = 1;
			extern void clear();

			extern bool isKeyPressed(unsigned int key);
			extern bool isKeyDown(unsigned int key);
			extern bool isKeyReleased(unsigned int key);

			extern std::string getTypedText(unsigned int transform = NONE);
			extern void setTypedText(std::string text);

			extern std::string getName(unsigned int button);

			extern unsigned int getLastKeyPressed();
			extern void setLastKeyPressed(unsigned int key);
		}

		namespace mouse
		{
			const unsigned int LEFT = 0x800;
			const unsigned int RIGHT = 0x801;
			const unsigned int MIDDLE = 0x802;

			extern void clear();

			extern int wheel();

			extern vec2di getPosition();
			extern int getX();
			extern int getY();

			extern bool hits(ww::Rectanglei rect);

			extern bool isButtonPressed(unsigned int button);
			extern bool isButtonDown(unsigned int button);
			extern bool isButtonReleased(unsigned int button);

			extern std::string getName(unsigned int button);
		}

		namespace touch
		{
			const unsigned int SWIPE_RIGHT = 0x1001;
			const unsigned int SWIPE_UP = 0x1001;
			const unsigned int SWIPE_LEFT = 0x1001;
			const unsigned int SWIPE_DOWN = 0x1001;

			extern void clearTouches();
			extern bool swiped(int direction);
			extern bool touched();
			extern bool touched(int x, int y, int x2, int y2);
			extern bool touchedArea(int x, int y, int w, int h);
			
			extern bool tapped();
			extern bool tapped(int x, int y, int x2, int y2);
			extern bool tappedArea(int x, int y, int w, int h);
			
			extern bool doubleTapped();
			extern bool doubleTapped(int x, int y, int x2, int y2);
			extern bool doubleTappedArea(int x, int y, int w, int h);
			
			extern unsigned int rawtouchCount();
			extern vec2di rawtouchLocation(unsigned int index);
		}

		namespace accelerometer
		{
			extern vec3df get();
			extern float getX();
			extern float getY();
			extern float getZ();
		}

		namespace iCade
		{
			const unsigned char RIGHT = 0x10001;
			const unsigned char UP = 0x10002;
			const unsigned char LEFT = 0x10003;
			const unsigned char DOWN = 0x10004;
			
			const unsigned char BUTTON1 = 0x10005;
			const unsigned char BUTTON2 = 0x10006;
			const unsigned char BUTTON3 = 0x10007;
			const unsigned char BUTTON4 = 0x10008;
			
			const unsigned char BUTTON5 = 0x10009;
			const unsigned char BUTTON6 = 0x1000A;
			const unsigned char BUTTON7 = 0x1000B;
			const unsigned char BUTTON8 = 0x1000C;

			extern bool isButtonPressed(unsigned int button);
			extern bool isButtonDown(unsigned int button);
			extern bool isButtonReleased(unsigned int button);

			extern std::string getName(unsigned char button);
		}

		namespace xbox360
		{
			extern bool isConnected(int controller);
			extern vec2df getLeftThumb(int controller);
			extern vec2df getRightThumb(int controller);
			extern float getLeftTrigger(int controller);
			extern float getRightTrigger(int controller);
			
			extern bool isButtonPressed(int controller, int id);
			extern bool isButtonReleased(int controller, int id);
			extern bool isButtonDown(int controller, int id);

			extern std::string getName(unsigned int button);
		}
	}
}

#include <SFML/Window/Keyboard.hpp>
namespace ww
{
	namespace input
	{
		namespace key
		{
			// all values stolen from http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
			const unsigned int Backspace = 0x08;
			const unsigned int Tab = 0x09;
			const unsigned int Return = 0X0D;
			const unsigned int Shift = 0x10;
			const unsigned int Control = 0x11;
			const unsigned int Alt = 0x12;
			const unsigned int Pause = 0x13;
			const unsigned int Capslock = 0x14;
			const unsigned int Escape = 0x1B;
			const unsigned int Space = 0x20;
			const unsigned int PageUp = 0x21;
			const unsigned int PageDown = 0x22;
			const unsigned int End = 0x23;
			const unsigned int Home = 0x24;
			const unsigned int Left = 0x25;
			const unsigned int Up = 0x26;
			const unsigned int Right = 0x27;
			const unsigned int Down = 0x28;
			//const unsigned int Select = 0x29;
			//const unsigned int Print = 0x2A;
			//const unsigned int Execute = 0x2B;
			//const unsigned int Snapshot = 0x2C;
			const unsigned int Insert = 0x2D;
			const unsigned int Delete = 0x2E;
			//const unsigned int Help = 0x2F;
			const unsigned int Num0 = 0x30;
			const unsigned int Num1 = 0x31;
			const unsigned int Num2 = 0x32;
			const unsigned int Num3 = 0x33;
			const unsigned int Num4 = 0x34;
			const unsigned int Num5 = 0x35;
			const unsigned int Num6 = 0x36;
			const unsigned int Num7 = 0x37;
			const unsigned int Num8 = 0x38;
			const unsigned int Num9 = 0x39;
			const unsigned int A = 0x41;
			const unsigned int B = 0x42;
			const unsigned int C = 0x43;
			const unsigned int D = 0x44;
			const unsigned int E = 0x45;
			const unsigned int F = 0x46;
			const unsigned int G = 0x47;
			const unsigned int H = 0x48;
			const unsigned int I = 0x49;
			const unsigned int J = 0x4A;
			const unsigned int K = 0x4B;
			const unsigned int L = 0x4C;
			const unsigned int M = 0x4D;
			const unsigned int N = 0x4E;
			const unsigned int O = 0x4F;
			const unsigned int P = 0x50;
			const unsigned int Q = 0x51;
			const unsigned int R = 0x52;
			const unsigned int S = 0x53;
			const unsigned int T = 0x54;
			const unsigned int U = 0x55;
			const unsigned int V = 0x56;
			const unsigned int W = 0x57;
			const unsigned int X = 0x58;
			const unsigned int Y = 0x59;
			const unsigned int Z = 0x5A;
			const unsigned int LWindows = 0x5B;
			const unsigned int RWindows = 0x5C;
			const unsigned int Applications = 0x5D;
			const unsigned int Sleep = 0x5F;
			const unsigned int Numpad0 = 0x60;
			const unsigned int Numpad1 = 0x61;
			const unsigned int Numpad2 = 0x62;
			const unsigned int Numpad3 = 0x63;
			const unsigned int Numpad4 = 0x64;
			const unsigned int Numpad5 = 0x65;
			const unsigned int Numpad6 = 0x66;
			const unsigned int Numpad7 = 0x67;
			const unsigned int Numpad8 = 0x68;
			const unsigned int Numpad9 = 0x69;
			const unsigned int NumpadMultiply = 0x6A;
			const unsigned int NumpadPlus = 0x6B;
			const unsigned int Separator = 0x6C;
			const unsigned int NumpadMinus = 0x6D;
			const unsigned int Decimal = 0x6E;
			const unsigned int NumpadSlash = 0x6F;
			const unsigned int F1 = 0x70;
			const unsigned int F2 = 0x71;
			const unsigned int F3 = 0x72;
			const unsigned int F4 = 0x73;
			const unsigned int F5 = 0x74;
			const unsigned int F6 = 0x75;
			const unsigned int F7 = 0x76;
			const unsigned int F8 = 0x77;
			const unsigned int F9 = 0x78;
			const unsigned int F10 = 0x79;
			const unsigned int F11 = 0x7A;
			const unsigned int F12 = 0x7B;
			const unsigned int F13 = 0x7C;
			const unsigned int F14 = 0x7D;
			const unsigned int F15 = 0x7E;
			const unsigned int F16 = 0x7F;
			const unsigned int F17 = 0x80;
			const unsigned int F18 = 0x81;
			const unsigned int F19 = 0x82;
			const unsigned int F20 = 0x83;
			const unsigned int F21 = 0x84;
			const unsigned int F22 = 0x85;
			const unsigned int F23 = 0x86;
			const unsigned int F24 = 0x87;
			const unsigned int LShift = 0xA0;
			const unsigned int RShift = 0xA1;
			const unsigned int LControl = 0xA2;
			const unsigned int RControl = 0xA3;
			const unsigned int Semicolon = 0xBA;
			const unsigned int Plus = 0xBB;
			const unsigned int Comma = 0xBC;
			const unsigned int Minus = 0xBD;
			const unsigned int Period = 0xBE;
			const unsigned int Backslash = 0xBF;
			const unsigned int Tilde = 0xC0;
			const unsigned int LBracket = 0xDB;
			const unsigned int Slash = 0xDC;
			const unsigned int RBracket = 0xDD;
			const unsigned int Quote = 0xDE;

			// CUSTOM?

			const unsigned int Equals = 0xDF;

			
		};
	};
};
#endif