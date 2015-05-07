#include <WadeWork/gfx.h>
#include <WadeWork/sys_defines.h>
#include <SFML/Window.hpp>

namespace ww
{
	namespace gfx
	{
		sf::Window *window = NULL;
		unsigned int window_width = 640;
		unsigned int window_height = 480;

		ww::gfx::RenderSubrect renderSubrect;

		RenderTarget *currentRenderTarget = NULL;
		bool FORCE_GL1 = false;

		GLfloat *savedMatrix;

#if PLATFORM_IOS
		extern void glBindFramebuffer0();
#endif


		bool supportsOpenGL2()
		{
			return false;
			static bool set = false;
			static bool GL2 = false;
			if (!set)
			{
				set = true;
#if PLATFORM_PC
				char* vers = (char*)glGetString(GL_VERSION);
				if (vers == NULL)
				{
					printf("An error occurred when obtaining OpenGL version: Version is null.\n");
					set = false;
				}
				else
				{
					GL2 = (!(atof(vers) < 2.0f));
					printf("OpenGL version is %s\n",vers);
					if (GL2)
						printf("GL2 = true\n");
					else
						printf("GL2 = false\n");
				}
#else
				std::string vers = (char*)glGetString(GL_VERSION);
				GL2 = (vers.find("OpenGL ES 2.0") != std::string::npos);

				NSLog(@"\n\n\n\n\n\n\n\n%s\n\n\n\n\n\n\n\n\n",vers.c_str());
				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"OpenGL String"
																message:[NSString stringWithUTF8String:(const char*)glGetString(GL_VERSION)]
															   delegate:nil
													  cancelButtonTitle:@"OK"
													  otherButtonTitles:nil];
				[alert show];
				[alert release];
#endif
				if (FORCE_GL1)
				{
					printf("Force OpenGL 1.\n"); // Not really. This just forces glew to not function, removing shaders and other things.
					GL2 = false;
				}
			}
			return GL2;
		}
		void setWindowTitle(std::string title)
		{
#if PLATFORM_IOS
			NSLog(@"Warning: setWindowTitle is unavailable on mobile devices.");
#else
			if (window)
				window->setTitle(title);
#endif
		}

		void setWindowSize(unsigned int width, unsigned int height)
		{
#if PLATFORM_IOS
			NSLog(@"Warning: setWindowSize is unavailable on mobile devices.");
#else
			window_width = width;
			window_height = height;
			if (window)
				window->setSize(sf::Vector2u(window_width,window_width));
#endif
		}

		ww::gfx::RenderSubrect getRenderSubrect()
		{
			return renderSubrect;
		}

		void setRenderSubrect(ww::gfx::RenderSubrect subrect)
		{
			renderSubrect = subrect;
			if (currentRenderTarget != NULL)
			{
				glScissor(subrect.x,subrect.y,subrect.width,subrect.height);
				glViewport(subrect.x,subrect.y,subrect.width,subrect.height);
			}
			else
			{
	#if PLATFORM_IOS
				subrect.x *= (int)[UIScreen mainScreen].scale;
				subrect.y *= (int)[UIScreen mainScreen].scale;
				subrect.width *= (int)[UIScreen mainScreen].scale;
				subrect.height *= (int)[UIScreen mainScreen].scale;
	#endif
				glScissor(subrect.x,window_height-(subrect.y+subrect.height),subrect.width,subrect.height);
				glViewport(subrect.x,window_height-(subrect.y+subrect.height),subrect.width,subrect.height);
			}
		}

		void display()
		{
			window->display();
		}

		void resetRenderTarget()
		{
			currentRenderTarget = NULL;
#if PLATFORM_PC
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#elif PLATFORM_IOS
			glBindFramebuffer0();
#endif
		}
		void setRenderTarget(RenderTarget *target)
		{
			currentRenderTarget = target;
			if (currentRenderTarget == NULL)
				resetRenderTarget();
			else
				currentRenderTarget->bind();
		}
		RenderTarget *getRenderTarget()
		{
			return currentRenderTarget;
		}

		void setBlendMode(unsigned int bm)
		{
			if (bm == ww::gfx::BM_NORMAL)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//, GL_ONE, GL_ONE);
				//glBlendFuncSeparate(GL_SRC_ALPHA,GL_DST_ALPHA,GL_ONE,GL_ONE);
				//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			}
			if (bm == ww::gfx::BM_ADD)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			}
			if (bm == ww::gfx::BM_SUBTRACT)
			{
			}
			if (bm == ww::gfx::BM_INVERT)
			{
				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			}
			if (bm == ww::gfx::BM_MULTIPLY)
			{
#if 1
				glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);//, GL_ONE, GL_ONE);
#else
				glBlendFuncSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
#endif
				//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//
			}
		}

		void setMatrix(ww::gfx::Shader *shader, GLint uniform, const GLfloat *matrix)
		{
			savedMatrix = (GLfloat*)matrix;
			if (shader == NULL)
				glLoadMatrixf(savedMatrix);
			else
				shader->setMatrix(uniform,savedMatrix);
		}

		GLfloat *getMatrix()
		{
			return savedMatrix;
		}
	};
};