#include <WadeWork/Sprite.h>
namespace ww
{
	namespace gfx
	{
		Sprite::Sprite()
		{
			vertices = new ww::gfx::Vertex[6]; // enough vertices for a face
			internalColor = 0xFFFFFFFF;
			internalX = 0.f;
			internalY = 0.f;
			internalWidth = 1;
			internalHeight = 1;
			subimageCount = 1;
		}

		void Sprite::setSize(unsigned int width, unsigned int height)
		{
			if (width != internalWidth || height != internalHeight)
			{
				internalWidth = width;
				internalHeight = height;
				for(int i=0;i<subimageCount;i++)
				{
					vertices[6*i+0].x = internalX+internalWidth;
					vertices[6*i+0].y = internalY+internalHeight;

					vertices[6*i+1].x = internalX+internalWidth;
					vertices[6*i+1].y = internalY;

					vertices[6*i+2].x = internalX;
					vertices[6*i+2].y = internalY;

					vertices[6*i+3].x = internalX+internalWidth;
					vertices[6*i+3].y = internalY+internalHeight;

					vertices[6*i+4].x = internalX;
					vertices[6*i+4].y = internalY;

					vertices[6*i+5].x = internalX;
					vertices[6*i+5].y = internalY+internalHeight;
				}
			}
		}

		ww::vec2dui Sprite::getSize()
		{
			return ww::vec2dui(internalWidth,internalHeight);
		}

		void Sprite::setSubimageUVRect(unsigned int subimage, ww::Rectanglef rect)
		{
			setSubimageUVRect(subimage,rect.x,rect.y,rect.width,rect.height);
		}
		void Sprite::setSubimageUVRect(unsigned int subimage, float u, float v, float w, float h)
		{
			if (subimage >= subimageCount)
				return;
			float L = min(u,u+w);
			float R = max(u,u+w);
			float T = min(v,v+h);
			float B = max(v,v+h);
			vertices[6*subimage+0].u = R;
			vertices[6*subimage+0].v = B;

			vertices[6*subimage+1].u = R;
			vertices[6*subimage+1].v = T;

			vertices[6*subimage+2].u = L;
			vertices[6*subimage+2].v = T;

			vertices[6*subimage+3].u = R;
			vertices[6*subimage+3].v = B;

			vertices[6*subimage+4].u = L;
			vertices[6*subimage+4].v = T;

			vertices[6*subimage+5].u = L;
			vertices[6*subimage+5].v = B;

			/*
			vertices[0] = ww::gfx::MakeVertex(width,height,0,0xFFFFFFFF,u2,v2);
			vertices[1] = ww::gfx::MakeVertex(width,0,0,0xFFFFFFFF,u2,v1);
			vertices[2] = ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,u1,v1);
			vertices[3] = ww::gfx::MakeVertex(width,height,0,0xFFFFFFFF,u2,v2);
			vertices[4] = ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,u1,v1);
			vertices[5] = ww::gfx::MakeVertex(0,height,0,0xFFFFFFFF,u1,v2);
			*/
		}

		ww::Rectanglef Sprite::getSubimageUVRect(unsigned int subimage)
		{
			if (subimage >= subimageCount)
				return ww::Rectanglef(0.f,0.f,0.f,0.f);
			return ww::Rectanglef(vertices[6*subimage+2].u,vertices[6*subimage+2].v,vertices[6*subimage+0].u-vertices[6*subimage+2].u,vertices[6*subimage+0].v-vertices[6*subimage+2].v);
		}

		void Sprite::setSubimageCount(unsigned int count)
		{
			// 6 = 6 tris per subimage. because I use GL_TRIANGLES
			if (count == 0)
				count = 1;
			ww::gfx::Vertex *newVertices = new ww::gfx::Vertex[count*6];
			memcpy(newVertices,vertices,6*min(count,subimageCount)*sizeof(ww::gfx::Vertex));
			// update the vertex count
			subimageCount = count;
			// save the current values
			int w = internalWidth;
			int h = internalHeight;
			float x = internalX;
			float y = internalY;
			unsigned int color = internalColor;
			// change them internally
			internalWidth -= 1;
			internalX -= 1.f;
			internalColor = color-1;
			// replace the array
			delete vertices;
			vertices = newVertices;
			// now call the change functions so all the new vertices get updated too!
			setSize(w,h);
			setColor(color);
			setPosition(x,y);
		}
		unsigned int Sprite::getSubimageCount()
		{
			return subimageCount;
		}

		void Sprite::setPosition(float x, float y)
		{
			if (x != internalX || y != internalY)
			{
				internalX = x;
				internalY = y;
				for(int i=0;i<subimageCount;i++)
				{
					vertices[6*i+0].x = internalX+internalWidth;
					vertices[6*i+0].y = internalY+internalHeight;

					vertices[6*i+1].x = internalX+internalWidth;
					vertices[6*i+1].y = internalY;

					vertices[6*i+2].x = internalX;
					vertices[6*i+2].y = internalY;

					vertices[6*i+3].x = internalX+internalWidth;
					vertices[6*i+3].y = internalY+internalHeight;

					vertices[6*i+4].x = internalX;
					vertices[6*i+4].y = internalY;

					vertices[6*i+5].x = internalX;
					vertices[6*i+5].y = internalY+internalHeight;
				}
			}
		}
		ww::vec2df Sprite::getPosition()
		{
			return ww::vec2df(internalX,internalY);
		}
		ww::gfx::Vertex *Sprite::getSubimageVertices(unsigned int subimage)
		{
			if (subimage >= subimageCount)
				return NULL;
			/*vertices[6*subimage+0].x = internalX+internalWidth;
			vertices[6*subimage+0].y = internalY+internalHeight;

			vertices[6*subimage+1].x = internalX+internalWidth;
			vertices[6*subimage+1].y = internalY;

			vertices[6*subimage+2].x = internalX;
			vertices[6*subimage+2].y = internalY;

			vertices[6*subimage+3].x = internalX+internalWidth;
			vertices[6*subimage+3].y = internalY+internalHeight;

			vertices[6*subimage+4].x = internalX;
			vertices[6*subimage+4].y = internalY;

			vertices[6*subimage+5].x = internalX;
			vertices[6*subimage+5].y = internalY+internalHeight;*/

			return &vertices[6*subimage];// + 6*subimage*sizeof(ww::gfx::Vertex);
		}
		
		void Sprite::setColor(unsigned int color)
		{
			if (color != internalColor)
			{
				internalColor = color;
				for(int i=0;i<6*subimageCount;i++)
					vertices[i].color = internalColor;
			}
		}
		unsigned int Sprite::getColor()
		{
			return internalColor;
		}
	} // namespace gfx
} // namespace ww
