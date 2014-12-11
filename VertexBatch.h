#ifndef __WadeWork__VertexBatch_h__
#define __WadeWork__VertexBatch_h__
#include <WadeWork/opengl.h>

#define min(a,b) ((a) < (b)?(a):(b))
#define max(a,b) ((a) > (b)?(a):(b))

namespace ww
{
	namespace gfx
	{
		const unsigned int MAX_VERTEXBATCH_TRIANGLE_COUNT = 0x1fff;

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

		typedef struct
		{
			ww::gfx::Vertex vertices[6]; // wow
		} Sprite;

		inline Sprite makeSprite(float x, float y, float width, float height, float u, float v, float w, float h)
		{
			float L = min(u,u+w);
			float R = max(u,u+w);
			float T = min(v,v+h);
			float B = max(v,v+h);
			Sprite spr;
			spr.vertices[0] = ww::gfx::MakeVertex(x+width,y+height,0,0xFFFFFFFF,R,B);
			spr.vertices[1] = ww::gfx::MakeVertex(x+width,y,0,0xFFFFFFFF,R,T);
			spr.vertices[2] = ww::gfx::MakeVertex(x,y,0,0xFFFFFFFF,L,T);
			spr.vertices[3] = ww::gfx::MakeVertex(x+width,y+height,0,0xFFFFFFFF,R,B);
			spr.vertices[4] = ww::gfx::MakeVertex(x,y,0,0xFFFFFFFF,L,T);
			spr.vertices[5] = ww::gfx::MakeVertex(x,y+height,0,0xFFFFFFFF,L,B);
			return spr;
		}

		class VertexBatch
		{
		private:
			int VERT_SIZE;
			GLuint vertexBuffer;
			GLuint vertexArray;
			void *vertices;
			unsigned int vertexCount;
			bool usesNormals;
			bool initialized;
			void init();
			bool dirty;
		public:
			static const unsigned int AttribPosition = 0;
			static const unsigned int AttribColor = 1;
			static const unsigned int AttribTexCoord0 = 2;
			static const unsigned int AttribNormal = 3;

			bool loadOBJ(const char *fname);
			VertexBatch(bool normals);
			VertexBatch();
			~VertexBatch();
			void pushsprite(Sprite *sprite);
			void pushvertex(Vertex v);
			void pushvertices(Vertex *vs, unsigned int number);
			void pushnvertex(NVertex nv);
			void pushnvertices(NVertex *nvs, unsigned int number);
			void *getVertices();
			unsigned int getVertexCount();
			bool hasNormals();
			void clear();
			void update();
			void draw(unsigned int vertexType = GL_TRIANGLES);

			void markAsDirty();

			void print();
		};
	} // namespace gfx
} // namespace ww
#endif