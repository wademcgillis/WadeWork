#ifndef __WadeWork__VertexBatch_h__
#define __WadeWork__VertexBatch_h__
#include <WadeWork/opengl.h>
#include <WadeWork/Sprite.h>

#define min(a,b) ((a) < (b)?(a):(b))
#define max(a,b) ((a) > (b)?(a):(b))

namespace ww
{
	namespace gfx
	{
		class VertexBatch
		{
		private:
			int VERT_SIZE;
			GLuint vertexBuffer;
			GLuint vertexArray;
			void *vertices;
			bool autoResizeVertexArray;
			unsigned int maxVertexCount;
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
			VertexBatch(bool normals, unsigned int maxVertCount, bool autoResizeVertices);
			VertexBatch(bool normals, unsigned int maxVertCount);
			VertexBatch(bool normals);
			VertexBatch();
			~VertexBatch();
			void pushsprite(ww::gfx::Sprite *sprite, unsigned int subimage = 0);
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