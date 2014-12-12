#ifndef __WadeWork__Sprite_h__
#define __WadeWork__Sprite_h__
#include <WadeWork/types.h>
#include <WadeWork/gfx.h>
namespace ww
{
	namespace gfx
	{
		class Sprite
		{
		private:
			ww::gfx::Vertex *vertices;
			unsigned int subimageCount;
			unsigned int internalWidth, internalHeight;
			float internalX, internalY;
			unsigned int internalColor;
		public:
			Sprite();

			void setSize(unsigned int width, unsigned int height);
			ww::vec2dui getSize();

			void setSubimageUVRect(unsigned int subimage, float u, float v, float w, float h);
			ww::Rectanglef getSubimageUVRect(unsigned int subimage);

			ww::vec2df getPosition();

			void setSubimageCount(unsigned int count);
			unsigned int getSubimageCount();

			void setPosition(float x, float y);
			ww::gfx::Vertex *getSubimageVertices(unsigned int subimage);
			
			unsigned int getColor();
			void setColor(unsigned int color);
		};
	}
}
#endif