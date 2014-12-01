//
//  Wumbo-Shader.cpp
//  Wumbo
//
//  Created by Wade McGillis on 11/23/13.
//  Copyright (c) 2013 Wade McGillis. All rights reserved.
//

#include <WadeWork/Shader.h>
#include <WadeWork/gfx.h>

namespace ww
{	
	namespace gfx
	{
		GLuint Shader::currentProgram = 99999;
		
		void Shader::builderSetProgram(GLuint prog)
		{
			program = prog;
		}
		void Shader::builderAddUniform(const GLchar *uniform)
		{
			//printf("builderAddUniform -> %i\n",glGetUniformLocation(program, uniform));
			uniforms.insert(std::pair<const GLchar*,GLint>(uniform,glGetUniformLocation(program, uniform)));
		}

		void Shader::makeActive()
		{
			if (!ww::gfx::supportsOpenGL2())
				return;
			if (currentProgram != program)
			{
				currentProgram = program;
				glUseProgram(program);
			}
		}
		void Shader::setActive(bool active)
		{
			if (active)
			{
				currentProgram = program;
				glUseProgram(program);
			}
			else
			{
				currentProgram = 0;
				glUseProgram(0);
			}
		}
		bool Shader::isActive()
		{
			return currentProgram == program;
		}

		GLuint Shader::getProgramID()
		{
			return program;
		}
		GLint Shader::getUniformIDFromName(const GLchar* uniformName)
		{
			if (uniforms.find(uniformName) != uniforms.end())
				return uniforms.find(uniformName)->second;
			return -1;
		}

		/*GLint Shader::getAttribLocation(const GLchar* attribName)
		{
			if (attribs.find(attribName) != attribs.end())
				return attribs.find(attribName)->second;
			return 0;
		}*/

		void Shader::getUniform_f(GLint uniformID, GLfloat *floats)
		{
			glGetUniformfv(program, uniformID, floats);
		}
		void Shader::getUniform_i(GLint uniformID, GLint *ints)
		{
			glGetUniformiv(program, uniformID, ints);
		}
		
		void Shader::getUniform_f(const GLchar *uniformID, GLfloat *floats)
		{
			GLint pos = getUniformIDFromName(uniformID);
			if (pos >= 0)
				getUniform_f(pos,floats);
			else
				printf("Uniform with name %s not found\n",uniformID);
			
		}
		void Shader::getUniform_i(const GLchar *uniformID, GLint *ints)
		{
			GLint pos = getUniformIDFromName(uniformID);
			if (pos >= 0)
				getUniform_i(pos,ints);
			else
				printf("Uniform with name %s not found\n",uniformID);
		}

		void Shader::setMatrix(GLint matrixUniformID, float *matrix)
		{
			glUniformMatrix4fv(matrixUniformID, 1, GL_FALSE, matrix);
		}
	} // namespace gfx
} // namespace ww