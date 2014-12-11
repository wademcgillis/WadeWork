#include <WadeWork/Buffer.h>
#include <stdio.h>  /* all I/O functions */

#if 1
#define DEBUGMSG(msg) printf("DEBUG %s:%i -- %s\n",__FILE__,__LINE__,msg);
#else
#define DEBUGMSG //
#endif

namespace ww
{
	unsigned int Buffer::WRITE = 1;
	unsigned int Buffer::READ = 0;
	
	Buffer::Buffer()
	{
		_bytes = NULL;
		_length = 0;
		mode = READ;
		position = 0;
		_arraySize = 0;
		flipped = false;
	}
	
	void binaryUCHAR(unsigned char number)
	{
		for(int i=0;i<sizeof(number)*8;i++)
		{
			if ((number >> i) & 0x1)
				printf("1");
			else
				printf("0");
		}
	}
	
	void binaryINT(int number)
	{
		for(int i=0;i<sizeof(number)*8;i++)
		{
			if ((number >> i) & 0x1)
				printf("1");
			else
				printf("0");
		}
	}
	
#if PLATFORM_APPLE
	bool Buffer::readfromfile(NSString *path)
	{
		return readfromfile([path UTF8String]);
	}
#endif
	bool Buffer::readfromfile(const char *path)
	{
#if PLATFORM_IOS
		NSMutableData *bobo = [NSMutableData dataWithContentsOfFile:[NSString stringWithFormat:@"%s",path]];
		if (bobo != nil)
		{
			_length = [bobo length];
			_bytes = (unsigned char*)[bobo bytes];
			_arraySize = _length;
			mode = READ;
			position = 0;
			/*printf("read %i bytes from files %s\n",_length,path);
			for(int i=0;i<_length;i++)
			{
				if (i % 48 == 0)
					printf("\n\t");
				else if (i % 4 == 0)
					printf(" | ");
				if ((unsigned char)_bytes[i] < 0x10)
					printf("0");
				printf("%X ",((unsigned char*)[bobo bytes])[i]);
			}
			printf("\n");
			printf("\n");*/
			return true;
		}
		else
		{
			DEBUGMSG("FILE I/O ERROR - readfromfile(): Failed to open file.\n");
			return false;
		}
#else
		FILE *f = fopen(path,"rb");
		if (f != NULL)
		{
			fseek(f,0,SEEK_END);
			_length = ftell(f);
			fseek(f,0,SEEK_SET);
			_arraySize = _length+256;
			_bytes = new unsigned char[_arraySize];
			memset(_bytes,0,_arraySize);
			fread((unsigned char*)_bytes, 1, _length, f);
			fclose(f);
			/*for(int i=0;i<_length;i++)
			{
				if (i % 48 == 0)
					printf("\n\t");
				else if (i % 4 == 0)
					printf(" | ");
				if (_bytes[i] < 10)
					printf("00");
				else if (_bytes[i] < 100)
					printf("0");
				printf("%i ",_bytes[i]);
			}*/
			mode = READ;
			position = 0;
			/*printf("read %i bytes from files %s\n",_length,path);
			for(int i=0;i<_length;i++)
			{
				if (i % 48 == 0)
					printf("\n\t");
				else if (i % 4 == 0)
					printf(" | ");
				if ((unsigned char)_bytes[i] < 0x10)
					printf("0");
				printf("%X ",(unsigned char)_bytes[i]);
			}
			printf("\n");
			printf("\n");*/
			return true;
		}
		else
			DEBUGMSG("FILE I/O ERROR - readfromfile(): Failed to open file.\n");
		return false;
#endif
	}
#if PLATFORM_APPLE
	bool Buffer::writetofile(const char *path)
	{
		return writetofile([NSString stringWithUTF8String:path]);
	}
	bool Buffer::writetofile(NSString *path)
	{
		FILE *f = fopen([path UTF8String],"wb");
		if (f == NULL)
		{
			NSFileHandle *file = [NSFileHandle fileHandleForWritingAtPath:path];
			if (file == nil)
			{
				if ([[NSFileManager defaultManager] createFileAtPath:path contents:nil attributes:nil])
				{
					f = fopen([path UTF8String],"wb");
					[file closeFile];
				}
				else
				{
					DEBUGMSG("FILE I/O ERROR - writetofile(): Failed to create new file.\n");
				}
			}
		}
#else
	bool Buffer::writetofile(const char *path)
	{
		FILE *f = fopen(path,"wb");
#endif
		if (f != NULL)
		{
			fwrite(_bytes, _length, 1, f);
			fclose(f);
			mode = WRITE;
			return true;
		}
		else
			DEBUGMSG("FILE I/O ERROR - writetofile(): Failed to open file.\n");
		return false;
	}
	Buffer::~Buffer()
	{
		//delete _bytes;
	}
	char *Buffer::getbytes()
	{
		return (char*)_bytes;
	}
	long Buffer::getlength()
	{
		return _length;
	}
	void Buffer::setbytes(char *bytes, unsigned int length)
	{
		if (_bytes != NULL)
			delete _bytes;
		_bytes = new unsigned char[length+1];
		memset(_bytes,0,length+1);
		memcpy(_bytes,bytes,length);
		_length = length;
		_arraySize = length;
	}
	
	void Buffer::readflippedbytes(bool flip)
	{
		flipped = flip;
	}

	void Buffer::setmode(unsigned int mod)
	{
		mode = mod;
		position = 0;
	}
	unsigned int Buffer::getmode()
	{
		return mode;
	}
	
	void Buffer::writeint8(char c)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeint8(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(char) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete old;
			// increase
			_arraySize += 256;
		}
		_bytes[position] = c;
		position += sizeof(char);
		_length += sizeof(char);
	}
	
	char Buffer::readint8()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readint8(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(char) <= _length)
		{
			position += sizeof(char);
			return _bytes[position-1];
		}
		else
		{
			DEBUGMSG("BUFFER READ ERROR - readint8(): Reading 1 byte puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	void Buffer::writeuint8(unsigned char c)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeuint8(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(unsigned char) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete old;
			// increase
			_arraySize += 256;
		}
		_bytes[position] = c;
		position += sizeof(char);
		_length += sizeof(char);
	}
	
	unsigned char Buffer::readuint8()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readuint8(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(unsigned char) <= _length)
		{
			position += sizeof(unsigned char);
			return _bytes[position-1];
		}
		else
		{
			DEBUGMSG("BUFFER READ ERROR - readuint8(): Reading 1 byte puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	void Buffer::writeint16(short s)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeint16(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(short) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&s)[0];
		_bytes[position + 1] = ((char*)&s)[1];
		position += sizeof(short);
		_length += sizeof(short);
	}
	
short Buffer::readint16()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readint16(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(short) <= _length)
		{
			short s = 0;
			if (flipped)
			{
				((char*)&s)[0] = _bytes[position+1];
				((char*)&s)[1] = _bytes[position+0];
			}
			else
			{
				((char*)&s)[0] = _bytes[position+0];
				((char*)&s)[1] = _bytes[position+1];
			}
			//printf("readint16() : %X %X => %X\n",((unsigned char*)&s)[0],((unsigned char*)&s)[1],s);
			position += sizeof(short);
			return s;
		}
		else
		{
			DEBUGMSG("BUFFER READ ERROR - readint16(): Reading 2 bytes puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	void Buffer::writeuint16(unsigned short s)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeuint16(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(unsigned short) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&s)[0];
		_bytes[position + 1] = ((char*)&s)[1];
		position += sizeof(unsigned short);
		_length += sizeof(unsigned short);
	}
	
	int yes = 0;
	unsigned short Buffer::readuint16()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readuint16(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(unsigned short) <= _length)
		{
			unsigned short s = 0;
			if (flipped)
			{
				((unsigned char*)&s)[0] = _bytes[position+1];
				((unsigned char*)&s)[1] = _bytes[position+0];
			}
			else
			{
				((unsigned char*)&s)[0] = _bytes[position+0];
				((unsigned char*)&s)[1] = _bytes[position+1];
			}
			//printf("readuint16() : %X %X => %X\n",((unsigned char*)&s)[0],((unsigned char*)&s)[1],s);
			position += sizeof(unsigned short);
			return s;
		}
		else
		{
			if (yes < 10)
			{
				DEBUGMSG("BUFFER READ ERROR - readuint16(): Reading 2 bytes puts the read cursor outside of the array size.\n");
				printf("position = %i\n",position);
				yes++;
			}
			return 0;
		}
	}
	
	
	
	
	void Buffer::writeint32(int i)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeint32(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(int) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&i)[0];
		_bytes[position + 1] = ((char*)&i)[1];
		_bytes[position + 2] = ((char*)&i)[2];
		_bytes[position + 3] = ((char*)&i)[3];
		position += sizeof(int);
		_length += sizeof(int);
	}
	
	int Buffer::readint32()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readint32(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(int) <= _length)
		{
			int s = 0;
			if (flipped)
			{
				((char*)&s)[0] = _bytes[position+3];
				((char*)&s)[1] = _bytes[position+2];
				((char*)&s)[2] = _bytes[position+1];
				((char*)&s)[3] = _bytes[position+0];
			}
			else
			{
				((char*)&s)[0] = _bytes[position+0];
				((char*)&s)[1] = _bytes[position+1];
				((char*)&s)[2] = _bytes[position+2];
				((char*)&s)[3] = _bytes[position+3];
			}
			position += sizeof(int);
			return s;
		}
		else
		{
			printf("BUFFER READ ERROR - readint32(): Reading 4 bytes puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	void Buffer::writeuint32(unsigned int u)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writeuint32(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(unsigned int) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete[] old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&u)[0];
		_bytes[position + 1] = ((char*)&u)[1];
		_bytes[position + 2] = ((char*)&u)[2];
		_bytes[position + 3] = ((char*)&u)[3];
		position += sizeof(unsigned int);
		_length += sizeof(unsigned int);
	}
	
	unsigned int Buffer::readuint32()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readuint32(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(unsigned int) <= _length)
		{
			unsigned int u = 0;
			if (flipped)
			{
				((char*)&u)[0] = _bytes[position+3];
				((char*)&u)[1] = _bytes[position+2];
				((char*)&u)[2] = _bytes[position+1];
				((char*)&u)[3] = _bytes[position+0];
			}
			else
			{
				((char*)&u)[0] = _bytes[position+0];
				((char*)&u)[1] = _bytes[position+1];
				((char*)&u)[2] = _bytes[position+2];
				((char*)&u)[3] = _bytes[position+3];
			}
			position += sizeof(unsigned int);
			return u;
		}
		else
		{
			printf("BUFFER READ ERROR - readuint32(): Reading 4 bytes puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	void Buffer::writefloat(float f)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writefloat(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(float) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete[] old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&f)[0];
		_bytes[position + 1] = ((char*)&f)[1];
		_bytes[position + 2] = ((char*)&f)[2];
		_bytes[position + 3] = ((char*)&f)[3];
		position += sizeof(float);
		_length += sizeof(float);
	}
	
	float Buffer::readfloat()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readfloat(): Buffer mode not set to READ\n");
			return 0.f;
		}
		if (position + sizeof(float) <= _length)
		{
			float f;
			if (flipped)
			{
				((char*)&f)[0] = _bytes[position + 3];
				((char*)&f)[1] = _bytes[position + 2];
				((char*)&f)[2] = _bytes[position + 1];
				((char*)&f)[3] = _bytes[position + 0];
			}
			else
			{
				((char*)&f)[0] = _bytes[position + 0];
				((char*)&f)[1] = _bytes[position + 1];
				((char*)&f)[2] = _bytes[position + 2];
				((char*)&f)[3] = _bytes[position + 3];
			}
			position += sizeof(float);
			return f;
		}
		else
		{
			printf("BUFFER READ ERROR - readfloat(): Reading 4 bytes puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	
	
	void Buffer::writedouble(double d)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writedouble(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + sizeof(double) >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+256];
			// copy
			memcpy(_bytes, old, _length);
			delete[] old;
			// increase
			_arraySize += 256;
		}
		_bytes[position + 0] = ((char*)&d)[0];
		_bytes[position + 1] = ((char*)&d)[1];
		_bytes[position + 2] = ((char*)&d)[2];
		_bytes[position + 3] = ((char*)&d)[3];
		_bytes[position + 4] = ((char*)&d)[4];
		_bytes[position + 5] = ((char*)&d)[5];
		_bytes[position + 6] = ((char*)&d)[6];
		_bytes[position + 7] = ((char*)&d)[7];
		position += sizeof(double);
		_length += sizeof(double);
	}
	
	double Buffer::readdouble()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readdouble(): Buffer mode not set to READ\n");
			return 0;
		}
		if (position + sizeof(double) <= _length)
		{
			double d;
			if (flipped)
			{
				((char*)&d)[0] = _bytes[position+7];
				((char*)&d)[1] = _bytes[position+6];
				((char*)&d)[2] = _bytes[position+5];
				((char*)&d)[3] = _bytes[position+4];
				((char*)&d)[4] = _bytes[position+3];
				((char*)&d)[5] = _bytes[position+2];
				((char*)&d)[6] = _bytes[position+1];
				((char*)&d)[7] = _bytes[position+0];
			}
			else
			{
				((char*)&d)[0] = _bytes[position];
				((char*)&d)[1] = _bytes[position+1];
				((char*)&d)[2] = _bytes[position+2];
				((char*)&d)[3] = _bytes[position+3];
				((char*)&d)[4] = _bytes[position+4];
				((char*)&d)[5] = _bytes[position+5];
				((char*)&d)[6] = _bytes[position+6];
				((char*)&d)[7] = _bytes[position+7];
			}
			position += sizeof(double);
			return d;
		}
		else
		{
			printf("BUFFER READ ERROR - readdouble(): Reading 8 bytes puts the read cursor outside of the array size.\n");
			return 0;
		}
	}
	
	
	void Buffer::writestring(std::string str)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writestring(): Buffer mode not set to WRITE\n");
			return;
		}
		// if the position plus the size of the element
		// is larger than what the array can hold
		if (position + str.length() >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+str.length()+256];
			// copy
			memcpy(_bytes, old, _length);
			delete[] old;
			// increase
			_arraySize += str.length()+256;
		}
		writeuint32(str.length());
		for(int i=0;i<str.length();i++)
			_bytes[position + i] = (char)str.at(i);
		position += str.length();
		_length += str.length();
	}
	
	std::string Buffer::readstring()
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readstring(): Buffer mode not set to READ\n");
			return 0;
		}
		unsigned int s = readuint32();
		if (s > 0)
		{
			if (position + s <= _length)
			{
				std::string str = "";
				for(int i=0;i<s;i++)
					str.push_back(_bytes[position + i]);
				position += str.length();
				return str;
			}
			else
			{
				printf("BUFFER READ ERROR - readstring(): Reading %i bytes puts the read cursor outside of the array size.\n",s);
				return "";
			}
		}
		return "";
	}
	
	void Buffer::writebytes(char *source, unsigned int length)
	{
		if (mode != WRITE)
		{
			DEBUGMSG("BUFFER WRITE ERROR - writebytes(): Buffer mode not set to WRITE\n");
			return;
		}
		if (position + length >= _arraySize)
		{
			unsigned char *old = _bytes;
			// make a new array and make it bigger
			_bytes = new unsigned char[_arraySize+length+256];
			// copy
			memcpy(_bytes, old, _length);
			delete[] old;
			// increase
			_arraySize += length+256;
		}
		memcpy(_bytes+position,source,length);
		position += length;
		_length += length;
	}
	void Buffer::readbytes(char *dest, unsigned int length)
	{
		if (mode != READ)
		{
			DEBUGMSG("BUFFER READ ERROR - readbytes(): Buffer mode not set to READ\n");
			return;
		}
		if (position + length <= _length)
		{
			memset(dest,0,length);
			memcpy(dest,_bytes + position,length);
			position += length;
		}
		else
			printf("BUFFER READ ERROR - readbytes(): Reading %i bytes puts the read cursor outside of the array size.\n",length);
		
	}
};