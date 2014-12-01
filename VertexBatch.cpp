//
//  Wumbo-VertexBatch.cpp
//  fjords_ios
//
//  Created by Wade McGillis on 4/19/14.
//  Copyright (c) 2014 Wade McGillis. All rights reserved.
//
#include <WadeWork/VertexBatch.h>
#include <WadeWork/gfx.h>
#if PLATFORM_PC
#define fmin(a,b) (a)<(b)?(a):(b)
#define fmax(a,b) (a)>(b)?(a):(b)
#endif

namespace ww
{
	namespace gfx
	{
		VertexBatch::VertexBatch(bool normals)
		{
			initialized = false;
			usesNormals = normals;
		}
		VertexBatch::VertexBatch()
		{
			initialized = false;
			usesNormals = false;
		}
		void *VertexBatch::getVertices()
		{
			return (void*)vertices;
		}
		void VertexBatch::init()
		{
			initialized = true;
			VERT_SIZE = 0; 
			vertexArray = -1;
			vertexBuffer = -1;
			if (usesNormals)
			{
				VERT_SIZE = sizeof(NVertex);
				vertices = new NVertex[MAX_VERTEXBATCH_TRIANGLE_COUNT * 3];
				memset(vertices,0,VERT_SIZE * MAX_VERTEXBATCH_TRIANGLE_COUNT * 3);
			}
			else
			{
				VERT_SIZE = sizeof(Vertex);
				vertices = new Vertex[MAX_VERTEXBATCH_TRIANGLE_COUNT * 3];
				memset(vertices,0,VERT_SIZE * MAX_VERTEXBATCH_TRIANGLE_COUNT * 3);
			}

			//printf("vert size = %u\n",VERT_SIZE);

			vertexCount = 3*ww::gfx::MAX_VERTEXBATCH_TRIANGLE_COUNT;

			if (ww::gfx::supportsOpenGL2())
			{
				printf("DOING VERTEX ARRAYS AND STUFF\n");
				glGenVertexArrays(1, &vertexArray);
				glBindVertexArray(vertexArray);
				glGenBuffers(1, &vertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, VERT_SIZE*vertexCount, vertices, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(AttribPosition);
				glVertexAttribPointer(AttribPosition, 3, GL_FLOAT, GL_FALSE, VERT_SIZE, (char*)(0));
				glEnableVertexAttribArray(AttribColor);
				glVertexAttribPointer(AttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERT_SIZE, (char*)(12));
				glEnableVertexAttribArray(AttribTexCoord0);
				glVertexAttribPointer(AttribTexCoord0, 2, GL_FLOAT, GL_FALSE, VERT_SIZE, (char*)(16));
				if (usesNormals)
				{
					glEnableVertexAttribArray(AttribNormal);
					glVertexAttribPointer(AttribNormal, 3, GL_FLOAT, GL_TRUE, VERT_SIZE, (char*)(24));
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
			else
				printf("NOPENGL2\n");
			vertexCount = 0;
			//glMapBufferOES(vertexbuffer, GL_WRITE_ONLY_OES);
		}
		void VertexBatch::pushvertex(Vertex v)//float x, float y, float z, unsigned int color, float u, float v)
		{
			((Vertex*)vertices)[vertexCount] = v;//MakeVertex(x,y,z,color,u,-v);
			vertexCount++;
		}
		void VertexBatch::pushvertices(Vertex *vs, unsigned int number)
		{
			for(unsigned int i=0;i<number;i++)
			{
				((Vertex*)vertices)[vertexCount] = vs[i];//MakeVertex(x,y,z,color,u,-v);
				vertexCount++;
			}
		}
		void VertexBatch::pushnvertex(NVertex nv)//float x, float y, float z, unsigned int color, float u, float v, float nx, float ny, float nz)
		{
			((NVertex*)vertices)[vertexCount] = nv;//MakeNVertex(x,y,z,color,u,-v,nx,ny,nz);
			vertexCount++;
		}
		void VertexBatch::pushnvertices(NVertex *nvs, unsigned int number)
		{
			for(unsigned int i=0;i<number;i++)
			{
				((NVertex*)vertices)[vertexCount] = nvs[i];//MakeVertex(x,y,z,color,u,-v);
				vertexCount++;
			}
		}
		void VertexBatch::clear()
		{
			if (!initialized)
				init();
			vertexCount = 0;
		}
		unsigned int VertexBatch::getVertexCount()
		{
			return vertexCount;
		}
		void VertexBatch::update()
		{
			if (ww::gfx::supportsOpenGL2())
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, VERT_SIZE*vertexCount, vertices, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
		void VertexBatch::print()
		{
			printf("====VERTEXBATCH %X====\n",(unsigned int)this);
			printf("VERT_SIZE\t%i\n",VERT_SIZE);
			printf("vertexCount\t%i\n",vertexCount);
			printf("vertexArray\t%i\n",vertexArray);
			printf("vertexBuffer\t%i\n",vertexBuffer);

		}
		void VertexBatch::markAsDirty()
		{
			dirty = true;
		}
		void VertexBatch::draw(unsigned int vertexType)
		{
			if (ww::gfx::supportsOpenGL2())
			{
				if (dirty)
				{
					dirty = false;
					update();
				}
				glBindVertexArray(vertexArray);
				glDrawArrays(vertexType, 0, vertexCount);
				glBindVertexArray(0);
			}
			else
			{
	#if !PLATFORM_IOS // fix this later
				glVertexPointer(3, GL_FLOAT, VERT_SIZE, ((char*)vertices));
				glColorPointer(4, GL_UNSIGNED_BYTE, VERT_SIZE, ((char*)vertices) + 12);
				glTexCoordPointer(2, GL_FLOAT, VERT_SIZE, ((char*)vertices) + 16);
				if (usesNormals)
					glNormalPointer(GL_FLOAT, VERT_SIZE, ((char*)vertices) + 24);
				glDrawArrays(vertexType,0,vertexCount);
	#endif
			}
		}

		typedef struct 
		{
			float x, y, z;
		} OBJXYZ;
		typedef struct
		{
			float u, v;
		} OBJUV;
		typedef struct
		{
			unsigned short V1;
			unsigned short V2;
			unsigned short V3;

			unsigned short T1;
			unsigned short T2;
			unsigned short T3;

			unsigned short N1;
			unsigned short N2;
			unsigned short N3;
		} OBJFACE;

		bool VertexBatch::loadOBJ(const char *fname)
		{
			FILE *f = fopen(fname,"rb");
			if (!f)
				return false;
			printf("Model %s opened successfully!\n",fname);
			char line[1024];
			OBJXYZ VERT[1024];
			unsigned int VERT_COUNT = 0;
			OBJUV TEX[1024];
			unsigned int TEX_COUNT = 0;
			OBJFACE FACE[1024];
			unsigned int FACE_COUNT = 0;
			while(!feof(f))
			{
				fgets(line,1023,f);
				printf("NEW LINE: %s",line);
				if (feof(f))
					break;
				char *tok = strtok(line," ");
				if (strcmp(tok,"v") == 0)
				{
					printf("\tWE GOT A VERTEX!");
					OBJXYZ vert;
					vert.x = atof(strtok(NULL," "));
					vert.y = atof(strtok(NULL," "));
					vert.z = atof(strtok(NULL," "));
					printf("\t%f %f %f\n",vert.x,vert.y,vert.z);
					VERT_COUNT++;
					VERT[VERT_COUNT] = vert;
				}
				if (strcmp(tok,"vt") == 0)
				{
					printf("\tWE GOT A TEXCOORD!");
					OBJUV tex;
					tex.u = atof(strtok(NULL," "));
					tex.v = 1.f-atof(strtok(NULL," "));
					printf("\t%f %f\n",tex.u,tex.v);
					TEX_COUNT++;
					TEX[TEX_COUNT] = tex;
				}
				if (strcmp(tok,"f") == 0)
				{
					printf("\tWE GOT A FACE!");
					char *f1 = strtok(NULL," ");
					char *f2 = strtok(NULL," ");
					char *f3 = strtok(NULL," ");

					OBJFACE face;
					face.V1 = atoi(strtok(f1,"/"));
					face.T1 = atoi(strtok(NULL,"/"));

					face.V2 = atoi(strtok(f2,"/"));
					face.T2 = atoi(strtok(NULL,"/"));

					face.V3 = atoi(strtok(f3,"/"));
					face.T3 = atoi(strtok(NULL,"/"));

					printf("\t%i/%i %i/%i %i/%i\n",face.V1,face.T1,face.V2,face.T2,face.V3,face.T3);
					FACE_COUNT++;
					FACE[FACE_COUNT] = face;
				}
			}
			fclose(f);
			this->clear();
			unsigned int C;
			for(int i=1;i<=FACE_COUNT;i++)
			{
				C = 0xFFFFFFFF;//0xFF000000 | (rand()%255)<<16 | (rand()%255)<<8 | (rand()%255)<<0;
				this->pushvertex(ww::gfx::MakeVertex(VERT[FACE[i].V1].x,VERT[FACE[i].V1].y,VERT[FACE[i].V1].z,C,TEX[FACE[i].T1].u,TEX[FACE[i].T1].v));
				this->pushvertex(ww::gfx::MakeVertex(VERT[FACE[i].V2].x,VERT[FACE[i].V2].y,VERT[FACE[i].V2].z,C,TEX[FACE[i].T2].u,TEX[FACE[i].T2].v));
				this->pushvertex(ww::gfx::MakeVertex(VERT[FACE[i].V3].x,VERT[FACE[i].V3].y,VERT[FACE[i].V3].z,C,TEX[FACE[i].T3].u,TEX[FACE[i].T3].v));
			}
			this->update();
			printf("==============\n");
			printf("  VERTEX COUNT: %i\n",VERT_COUNT);
			printf("TEXCOORD COUNT: %i\n",TEX_COUNT);
			//printf("  NORMAL COUNT: %i\n",NORM_COUNT);
			printf("FACE COUNT: %i\n",FACE_COUNT);
			
			// GENERATE THE MODEL HERE
			return true;
		}
	} // namespace gfx
} // namespace ww