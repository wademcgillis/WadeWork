#ifndef __WadeWork__gfx_h__
#define __WadeWork__gfx_h__
#include <WadeWork/sys_defines.h>
#include <WadeWork/opengl.h>

#include <WadeWork/types.h>
#include <WadeWork/RenderTarget.h>
#include <WadeWork/Shader.h>
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

		extern ww::Rectanglei getRenderSubrect();
		extern void setRenderSubrect(ww::Rectanglei subrect);

		extern void resetRenderTarget();
		extern void setRenderTarget(RenderTarget *target);
		extern RenderTarget *getRenderTarget();

		extern void display();

		extern void setMatrix(ww::gfx::Shader *shader, GLint uniform, const GLfloat *matrix);

		typedef struct
		{
			float x, y, z;
			unsigned int color;
			float u, v;
		} Vertex;
		inline Vertex MakeVertex(float x, float y, float z, unsigned int color, float u, float v)
		{
			Vertex vert;
			vert.x = x;
			vert.y = y;
			vert.z = z;
			vert.color = color;
			vert.u = u;
			vert.v = v;
			return vert;
		}
		typedef struct
		{
			float x, y, z;
			unsigned int color;
			float u, v;
			float nx, ny, nz;
		} NVertex;
		inline NVertex MakeNVertex(float x, float y, float z, unsigned int color, float u, float v, float nx, float ny, float nz)
		{
			NVertex nvert;
			nvert.x = x;
			nvert.y = y;
			nvert.z = z;
			nvert.color = color;
			nvert.u = u;
			nvert.v = v;
			nvert.nx = nx;
			nvert.ny = ny;
			nvert.nz = nz;
			return nvert;
		}
	};
};

#include <WadeWork/Sprite.h>
#include <WadeWork/VertexBatch.h>
#include <WadeWork/Texture.h>
#include <WadeWork/ShaderBuilder.h>
#endif