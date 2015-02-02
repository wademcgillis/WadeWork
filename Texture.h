#ifndef __WadeWork__Texture_h__
#define __WadeWork__Texture_h__
#include <WadeWork/opengl.h>
namespace ww
{
	namespace gfx
	{
		class Texture
		{
		private:
			bool notMyPointer;
			static GLuint sharedBoundTexture;
			unsigned int *dataPtr;
			GLuint texture;
			int width;
			int height;
			GLuint apparentWidth;
			GLuint apparentHeight;
			int loadType;
			bool textureLoaded;
#if PLATFORM_OSX || PLATFORM_IOS
			NSString *filepath;
#elif PLATFORM_WINDOWS
			std::string filepath;
			unsigned char* filepointer;
			unsigned int filesize;
#endif	
		public:
			Texture();
			~Texture();
			Texture(std::string fname);
			
			bool load(std::string fname);
#if PLATFORM_OSX || PLATFORM_IOS
			Texture(NSString *fname);
			bool load(NSString *fname);
#elif PLATFORM_WINDOWS
			Texture(const unsigned char* pointer, unsigned int size);
			bool load(const unsigned char* pointer, unsigned int size);
#endif
			Texture(GLuint texID, unsigned int width, unsigned int height);
			// ONLY USE FORCEDATA IF YOUR DATA IS POWER OF TWO AND THE CORRECT DIMENSIONS.
			bool loadLinearFromMemory(unsigned int *source, unsigned int w, unsigned int h, bool forceData = false);
			
			void setLinearInterpolation(bool linear);

			void reload();
			void unload();
			GLuint getTexture();
			GLuint getWidth();
			GLuint getHeight();
			
			unsigned int get(GLuint x, GLuint y);
			unsigned int getRGBPixel(GLuint x, GLuint y);
			unsigned int getBGRPixel(GLuint x, GLuint y);
			void setBGRPixel(GLuint x, GLuint y, unsigned int color);
			void setRGBPixel(GLuint x, GLuint y, unsigned int color);
			// THIS ACTS LIKE GAMEMAKER'S DRAW_RECTANGLE. X2 and Y2 AREN'T INCLUDED IN THE PIXEL SETTING!
			void setRGBRect(GLuint x1, GLuint y1, GLuint x2, GLuint y2, unsigned int color);
			void update();
			void update(unsigned int *source, unsigned int w, unsigned int h);

			void setApparentSize(GLuint w, GLuint h);
			
			void bind();
		};
	} // namespace gfx
} // namespace ww
#endif