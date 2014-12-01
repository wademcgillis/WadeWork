#include <WadeWork/gfx.h>

extern void glBindFramebuffer0();

namespace ww
{
	namespace gfx
	{
	//
	//		That function below, removeBind, is there because
	//		Visual Studio throws a hissy fit for some reason if it's a class method.
	//		Sorry!
	//
#if PLATFORM_IOS
		void removeBind()
		{
			glBindFramebuffer0();
			//glBindRenderbuffer(GL_RENDERBUFFER, 0);
			//glBindFramebuffer(GL_FRAMEBUFFER,zero);
			//glBindRenderbuffer(GL_RENDERBUFFER, zero);
		}
#else
		void removeBind()
		{
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
#endif

		void RenderTarget::bind()
		{
			if (!GL2)
			{
				printf("RenderTarget::bind is not available in OpenGL 1.x\n");
				return;
			}
#if PLATFORM_IOS
			glBindFramebuffer(GL_FRAMEBUFFER, frameID);
			//glBindRenderbuffer(GL_RENDERBUFFER, renderID);
#else
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameID);
#endif
		}

		void RenderTarget::initialize(unsigned int ww, unsigned int hh)
		{
			//printf("----- initialize()\n");
#if 1//PLATFORM_IOS
			if (ww::gfx::supportsOpenGL2())
				GL2 = true;
			width = ww;
			height = hh;
			texture = new Texture();
			unsigned char *mem = new unsigned char[4*ww*hh];
			memset(mem,0,4*ww*hh);
			/*for(int i=0;i<4*ww*hh;i += 4)
			{
				mem[i+0] = rand()%255;//128+127*sin(((float)i)/8.f);//rand()%128 + ((i > 0)?mem[i-1]/2:0);
				mem[i+1] = rand()%255;//128+127*cos(((float)i)/8.f);
				mem[i+2] = rand()%255;//mem[i + 0] ^ mem[i + 1];
				mem[i+3] = 0;//255;
			}*/
			texture->loadLinearFromMemory((unsigned int*)mem, ww, hh);
			textureID = texture->getTexture();

			int inwidth = texture->getWidth();
			int inheight = texture->getHeight();
			//printf("TextureID of the texture to be bound to the framebuffer object is %u\n",textureID);
			
			glBindTexture(GL_TEXTURE_2D, textureID);
#if PLATFORM_IOS
			glGenFramebuffers(1, &frameID);
			glBindFramebuffer(GL_FRAMEBUFFER, frameID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
#else
			glGenFramebuffersEXT(1, &frameID); // Generate one frame buffer and store the ID in fbo
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameID); // Bind our frame buffer
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID,0);

GLuint fb = 0;

#if 0
glGenRenderbuffers(1, &fb);
glBindRenderbuffer(GL_RENDERBUFFER, fb);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, inwidth,inheight);
glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb);
#endif

#if 1
glGenRenderbuffers(1, &fb);
glBindRenderbuffer(GL_RENDERBUFFER, fb);
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, inwidth,inheight);
glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb);
#endif

#if 0
glBindTexture(GL_TEXTURE_2D, fb);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, inwidth, inheight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, fb, 0);
#endif

#endif
			
			
			GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE)
#if PLATFORM_APPLE
				NSLog(@"failed to make complete framebuffer object\n\t%s", glGetString(status));
#else
				printf("failed to make complete framebuffer object\n\t%s", glGetString(status));
#endif
			glBindTexture(GL_TEXTURE_2D, 0);
			removeBind();
			//printf("renderbuff %i tex %i\n",frameID,textureID);
#endif
			//printf("----- END initialize()\n");
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
#if 0
			//glGenFramebuffers = wglGetProcAddress("glGenFramebuffers");;
			if (rend->getGLVersion() == 2)
				GL2 = true;
			width = ww;
			height = hh;
			if (!GL2)
			{
				printf("RenderTarget::initialize is not available in OpenGL 1.x\n");
				return;
			}
			initialized = true;
			
			texture = new Texture();
			texture->loadLinearFromMemory(new unsigned char[4*ww*hh], ww, hh);
			
			textureID = texture->getTexture();
			glBindTexture(GL_TEXTURE_2D, textureID);
#if PLATFORM_IOS
			glGenFramebuffers(1, &frameID); // Generate one frame buffer and store the ID in fbo
			glBindFramebuffer(GL_FRAMEBUFFER, frameID); // Bind our frame buffer
			
				glGenRenderbuffers(1, &renderID);
				glBindRenderbuffer(GL_RENDERBUFFER, renderID);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderID);
				
		
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
					
			GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if(status != GL_FRAMEBUFFER_COMPLETE)
				NSLog(@"failed to make complete framebuffer object:\n\t%s", glGetString(status));
			
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer
#else
			glGenFramebuffersEXT(1, &frameID); // Generate one frame buffer and store the ID in fbo
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameID); // Bind our frame buffer  
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureID,0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0); // Unbind our frame buffer
#endif

			glBindTexture(GL_TEXTURE_2D, 0);
			printf("renderbuff %i tex %i\n",frameID,textureID);
#endif

			delete mem;
		}

		RenderTarget::RenderTarget(unsigned int ww, unsigned int hh)
		{
			frameID = 0;
			texture = 0;
			initialized = false;
			GL2 = false;
			initialize(ww,hh);
		}

		RenderTarget::RenderTarget()
		{
			frameID = 0;
			texture = 0;
			initialized = false;
			width = 1;
			height = 1;
			GL2 = false;	
		}

		unsigned int RenderTarget::getWidth()
		{
			return width;
		}
		unsigned int RenderTarget::getHeight()
		{
			//printf("height = %u\n",height);
			return height;
		}

		Texture* RenderTarget::getTexture()
		{
			if (!GL2)
			{
				printf("RenderTarget::getTexture is not available in OpenGL 1.x\n");
				return NULL;
			}
			return texture;
		}
	} // namespace gfx
} // namespace ww