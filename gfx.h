#ifndef __WadeWork__gfx_h__
#define __WadeWork__gfx_h__
#include <WadeWork/sys_defines.h>
#include <WadeWork/opengl.h>

#include <WadeWork/RenderTarget.h>
namespace ww
{
	namespace gfx
	{
		const unsigned int BM_NORMAL = 0;
		const unsigned int BM_ADD = 1;
		const unsigned int BM_SUBTRACT = 2;
		const unsigned int BM_INVERT = 3;
		const unsigned int BM_MULTIPLY = 4;

		extern void setBlendMode(unsigned int bm);

		extern bool supportsOpenGL2();

		extern void setWindowSize(unsigned int width, unsigned int height);

		extern void setFramerate(unsigned int rate);
		extern unsigned int getFramerate();

		extern void resetRenderTarget();
		extern void setRenderTarget(RenderTarget *target);
		extern RenderTarget *getRenderTarget();

		extern void display();
	};
};


#include <WadeWork/VertexBatch.h>
#include <WadeWork/Texture.h>
#include <WadeWork/Shader.h>
#include <WadeWork/ShaderBuilder.h>
#endif