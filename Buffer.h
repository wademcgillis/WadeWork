#ifndef __WadeWork__Buffer_h__
#define __WadeWork__Buffer_h__
#include <WadeWork/sys_defines.h>

namespace ww
{
	class Buffer
	{
	private:
		bool flipped;
		unsigned char *_bytes;
		unsigned int _arraySize;
		long _length;
		
		unsigned int mode;
		unsigned int position;
	public:
		static unsigned int WRITE;
		static unsigned int READ;
		Buffer();
		~Buffer();
#if PLATFORM_APPLE
		bool readfromfile(NSString *path);
		bool writetofile(NSString *path);
#endif
		bool readfromfile(const char *path);
		bool writetofile(const char *path);

		char *getbytes();
		long getlength();
		void setbytes(char *bytes, unsigned int length);
		
		void setmode(unsigned int mode);
		unsigned int getmode();
		
		void writeint8(char c);
		char readint8();
		void writeuint8(unsigned char c);
		unsigned char readuint8();
		
		void writeint16(short s);
		short readint16();
		void writeuint16(unsigned short s);
		unsigned short readuint16();
		
		void writeint32(int i);
		int readint32();
		void writeuint32(unsigned int i);
		unsigned int readuint32();
		
		void writefloat(float f);
		float readfloat();
		
		void writedouble(double d);
		double readdouble();
		
		void writestring(std::string str);
		std::string readstring();
	
		void writebytes(char *bytes, unsigned int length);
		void readbytes(char *dest, unsigned int length);
	};
};
#endif