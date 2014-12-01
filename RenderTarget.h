#ifndef __Wumbo_RenderTarget_h__
#define __Wumbo_RenderTarget_h__
#include <WadeWork/opengl.h>
#include <WadeWork/Texture.h>
namespace ww
{
	namespace gfx
	{
		class RenderTarget
		{
		private:
			GLuint renderID;
			GLuint textureID;
			GLuint frameID;
			Texture *texture;
			bool initialized;
			unsigned int width;
			unsigned int height;
			bool GL2;
		public:
			RenderTarget(unsigned int width, unsigned int height);
			RenderTarget();
			void initialize(unsigned int width, unsigned int height);
			Texture *getTexture();

			unsigned int getWidth();
			unsigned int getHeight();

			void bind();
		};
	}
};
#endif