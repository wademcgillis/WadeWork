#include <WadeWork/Texture.h>
#if PLATFORM_PC
#include <WadeWork/ext/stb_image/stb_image.h>
#endif

namespace ww
{
	namespace gfx
	{
		GLuint Texture::sharedBoundTexture = 0;
		GLuint Texture::getWidth()
		{
			return apparentWidth;
		}

		GLuint Texture::getHeight()
		{
			return apparentHeight;
		}

		GLuint Texture::getTexture()
		{
			return texture;
		}

		Texture::Texture(GLuint texID, unsigned int width, unsigned int height)
		{
			dataPtr = NULL;
			width = width;
			height = height;
			apparentWidth = width;
			apparentHeight = height;
			texture = texID;
		}
		Texture::Texture()
		{
			dataPtr = NULL;
			loadType = 0;
			textureLoaded = false;
			width = 1;
			height = 1;
			apparentWidth = width;
			apparentHeight = height;
#if PLATFORM_OSX || PLATFORM_IOS
			filepath = @""; 
#else
			filepath = "";
#endif
		}

#if PLATFORM_OSX || PLATFORM_IOS
		Texture::Texture(NSString *fname)
		{
			textureLoaded = false;
			load(fname);
		}
#endif
		
		Texture::Texture(std::string fname)
		{
			textureLoaded = false;
			load(fname);
		}
		
		void Texture::setLinearInterpolation(bool linear)
		{
			if (linear)
			{
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
		}
		

		bool Texture::loadLinearFromMemory(unsigned int *source, unsigned int dataWidth, unsigned int dataHeight, bool forceData)
		{
			if (textureLoaded)
				return true;
			loadType = 0;
			textureLoaded = true;
#if PLATFORM_OSX || PLATFORM_IOS
			filepath = @"";
#else
			filepath = "";
#endif
			width = pow(2,ceil(log((float)dataWidth)/log(2.f)));
			height = pow(2,ceil(log((float)dataHeight)/log(2.f)));
			//printf("loadLinearFromMemory adjusted width and height:\n\t%i x %i\n",width,height);
			apparentWidth = width;
			apparentHeight = height;
			if (forceData)
				dataPtr = source;
			else
				dataPtr = new unsigned int[width*height];
			
			for(int i=0;i<dataWidth;i++)
				for(int j=0;j<dataHeight;j++)
					dataPtr[width*j + i] = source[dataWidth*j + i];
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			return true;
		}
		
		
#if PLATFORM_OSX || PLATFORM_IOS
		bool Texture::load(std::string fname)
		{
			return load([NSString stringWithUTF8String:fname.c_str()]);
		}
		
		bool Texture::load(NSString *fname)
#else
		bool Texture::load(std::string fname)
#endif
		{
			if (textureLoaded)
				return true;
			loadType = 0;
			textureLoaded = true;
			filepath = fname;
#if PLATFORM_IOS
			NSString* path = fname;//[NSString stringWithUTF8String:fname.c_str()];
			NSError *err = nil;
			NSData *data = [NSData dataWithContentsOfFile:path options:nil error:&err];
			if (data == nil)
			{
				texture = 0;
				NSLog(@"GLtexture::load() could not find file at: %@", path);
				NSLog([[NSBundle mainBundle] resourcePath]);
				printf("%s\n", [[err description] UTF8String]);
				return false;
			}    
			UIImage *uimg = [UIImage imageWithData:data];
			CGImageRef image = [uimg CGImage];
			NSLog(@"GLtexture::load() success: %@. Dimensions: %i, %i",path,(int)CGImageGetWidth(image),(int)CGImageGetHeight(image));
			width = CGImageGetWidth(image);
			height = CGImageGetHeight(image);
			apparentWidth = width;
			apparentHeight = height;
			CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			void *imageData = malloc( height * width * 4);// ,1);
			CGContextRef context = CGBitmapContextCreate( imageData, width, height, 8, 4 * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
			// Flip the Y-axis
			CGContextTranslateCTM (context, 0, height);
			CGContextScaleCTM (context, 1.0, -1.0);
			CGColorSpaceRelease( colorSpace );
			CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), image );
#else
		unsigned char *imageData;
#if PLATFORM_OSX
			if (!(imageData = stbi_load([fname UTF8String],&width,&height,NULL,false)))
			{
			printf("Error loading file: %s\n",[fname UTF8String]);
#else
			if (!(imageData = stbi_load(fname.c_str(),&width,&height,NULL,false)))
			{
				printf("Error loading file: %s\n",fname.c_str());
#endif
				return false;
			}
			/*img.flipVertically();
			const sf::Uint8 *imageData = img.getPixelsPtr();
			width = img.getSize().x;
			height = img.getSize().y;*/
			apparentWidth = width;
			apparentHeight = height;
			/*unsigned char *imageData2 = new unsigned char[width*height*4];
			memcpy(imageData2,imageData,width*height*4);


			unsigned char *source = &imageData2[width*(height-1)*4];
			unsigned char *dest = &imageData[0];

			for(int i=0;i<height;i++)
			{
				memcpy(dest,source,width*4);
				source -= width * 4;
				dest += width * 4;
			}
			delete imageData2;*/
			/*width*height*4 - width*4*(i+1)


			0-7			i = 0;
			8-15		i = 1;
			16-23		i = 2;
			24-31		i = 3
			32-39
			40-47
			48-55
			56-63*/
			//delete imageData;
			//imageData = imageData2;
#endif
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#if PLATFORM_IOS
			CGContextRelease(context);
#endif
			dataPtr = (unsigned int*)imageData;
			return true;
		}
#define POS x+width*((height-1)-y)
		unsigned int Texture::getRGBPixel(GLuint x, GLuint y)
		{
			if (dataPtr == NULL)
				return 0x00000000;
			return dataPtr[POS];
		}
			
			
			
			
		void Texture::update()
		{
			if (dataPtr != NULL)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, dataPtr);
		}
		void Texture::update(unsigned int *source, unsigned int dataWidth, unsigned int dataHeight)
		{
			
			for(unsigned int i=0;i<width;i++)
				for(unsigned int j=0;j<height;j++)
				{
					if (i < dataWidth && j < dataHeight)
						dataPtr[width*j + i] = source[dataWidth*(j) + i];
					else
						dataPtr[width*j + i] = 0x00000000;
				}
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, dataPtr);
		}
				
		void Texture::setRGBPixel(GLuint x, GLuint y, unsigned int color)
		{
			if (dataPtr == NULL)
				return;
			dataPtr[POS] = color;
		}
		void Texture::setBGRPixel(GLuint x, GLuint y, unsigned int color)
		{
			if (dataPtr == NULL)
				return;
			dataPtr[POS] = (((color >> 24) & 0xFF) << 24) | (((color >> 16) & 0xFF)) | (((color >> 8) & 0xFF) << 8) | ((color & 0xFF) << 16);
		}
		unsigned int Texture::getBGRPixel(GLuint x, GLuint y)
		{
			if (dataPtr == NULL)
				return 0x00000000;
#define POS x+width*((height-1)-y)
			return (((dataPtr[POS] >> 24) & 0xFF) << 24) | (((dataPtr[POS] >> 16) & 0xFF)) | (((dataPtr[POS] >> 8) & 0xFF) << 8) | ((dataPtr[POS] & 0xFF) << 16);
		}
		unsigned int Texture::get(GLuint x, GLuint y)
		{
			if (dataPtr == NULL)
				return 0x00000000;
#define POS x+width*((height-1)-y)
			return (((dataPtr[POS] >> 24) & 0xFF) << 24) | (((dataPtr[POS] >> 16) & 0xFF)) | (((dataPtr[POS] >> 8) & 0xFF) << 8) | ((dataPtr[POS] & 0xFF) << 16);
		}
#undef POS
#if PLATFORM_WINDOWS
		Texture::Texture(const unsigned char* pointer, unsigned int size)
		{
			textureLoaded = false;
			load(pointer,size);
		}

		bool Texture::load(const unsigned char* pointer, unsigned int size)
		{
			/*
			if (textureLoaded)
				return false;
			loadType = 1;
			filepointer = (sf::Uint8*) pointer;
			filesize = size;
			textureLoaded = true;
			printf("Loading from memory.");
			sf::Image img;
			if (!img.loadFromMemory(pointer,size))
			{
				printf("Error loading memory: %i",pointer);
				return false;
			}
			img.flipVertically();
			const sf::Uint8 *imageData = img.getPixelsPtr();
			width = img.getSize().x;
			height = img.getSize().y;
			apparentWidth = width;
			apparentHeight = height;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST);
			return false;*/
			return false;
		}
#endif


		void Texture::reload()
		{
			unload();
#if PLATFORM_WINDOWS
			if (loadType == 0)
				load(filepath);
			if (loadType == 1)
				load(filepointer,filesize);
#else
			load(filepath);
#endif
		}

		void Texture::unload()
		{
			if (textureLoaded)
			{
				glDeleteTextures(1,&texture);
				textureLoaded = false;
			}
		}
			
		void Texture::bind()
		{
			if (sharedBoundTexture != texture)
			{
				sharedBoundTexture = texture;
				glBindTexture(GL_TEXTURE_2D, texture);
			}
		}

		void Texture::setApparentSize(GLuint w, GLuint h)
		{
			apparentWidth = w;
			apparentHeight = h;
		}
	} // namespace gfx
} // namespace ww