#ifndef __WadeWork__Shader_h__
#define __WadeWork__Shader_h__
#include <iostream>
#include <map>
#include <WadeWork/opengl.h>

namespace ww
{
	namespace gfx
	{
		class Shader
		{
		private:
			static GLuint currentProgram;
			GLuint program;
			std::map<const GLchar*,GLint> uniforms;
		public:
			void builderSetProgram(GLuint prog);
			void builderAddUniform(const char *uniform);
			
			void makeActive();
			void setActive(bool active);
			bool isActive();
			
			GLuint getProgramID();
			GLint getUniformIDFromName(const GLchar* uniformID);
			
			void getUniform_f(const GLchar *uniformID, GLfloat *floats);
			void getUniform_i(const GLchar *uniformID, GLint *ints);
			void getUniform_f(GLint uniformID, GLfloat *floats);
			void getUniform_i(GLint uniformID, GLint *ints);

			void setMatrix(GLint matrixUniformID, float *matrix);
		};
	}
}
#endif
