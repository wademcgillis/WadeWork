//
//  Wumbo-Shader.cpp
//  Wumbo
//
//  Created by Wade McGillis on 11/23/13.
//  Copyright (c) 2013 Wade McGillis. All rights reserved.
//

#include <WadeWork/ShaderBuilder.h>
#include <WadeWork/VertexBatch.h>
#include <WadeWork/gfx.h>

/*
				if (strcmp(path,DEFAULT_FRAGMENT_SHADER_NOLIGHTING.c_str()) == 0)
				{
					source = "//#version 120\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nuniform sampler2D texture;\r\nvoid main()\r\n{\r\n\tgl_FragColor = vertexColor * texture2D(texture,texcoord0frag);\r\n}\0";
				}
				else if (strcmp(path,DEFAULT_VERTEX_SHADER_NOLIGHTING.c_str()) == 0)
				{
					source = "//#version 120\r\nattribute vec4 position;\r\nattribute vec4 color;\r\nattribute vec2 texcoord0;\r\nuniform mat4 matrix;\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nvoid main()\r\n{\r\n\tvertexColor = color;\r\n\ttexcoord0frag = texcoord0;\r\n\tgl_Position = matrix * position;\r\n}\0";
				}
				else
				{
					printf("Special Shader path \"%s\" is a special shader but isn't supported.\n",path);
				}
				len = strlen(source);
*/

namespace ww
{
	namespace gfx
	{
		char *file_alloc_contents(const char *path)
		{
			FILE *f = fopen(path,"rb");
			if (f == NULL)
				return NULL;
			fseek(f,0,SEEK_END);
			long len = ftell(f);
			fseek(f,0,SEEK_SET);
			char *contents = new char[len+1];
			memset(contents,0,len+1);
			fread(contents, len, 1, f);
			fclose(f);
			return contents;
		}

		Shader *ShaderBuilder::genericShader(unsigned int shaderID)
		{
			if (shaderID == DEFAULT_SHADER_NOLIGHTING)
			{
				ShaderBuilder builder;
				builder.loadFragmentShader(NULL,"//#version 120\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nuniform sampler2D texture;\r\nvoid main()\r\n{\r\n\tgl_FragColor = vertexColor * texture2D(texture,texcoord0frag);\r\n}\0");
				builder.loadVertexShader(NULL,"//#version 120\r\nattribute vec4 position;\r\nattribute vec4 color;\r\nattribute vec2 texcoord0;\r\nuniform mat4 matrix;\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nvoid main()\r\n{\r\n\tvertexColor = color;\r\n\ttexcoord0frag = texcoord0;\r\n\tgl_Position = matrix * position;\r\n}\0");
				builder.addAttrib(ww::gfx::VertexBatch::AttribPosition,"position");
				builder.addAttrib(ww::gfx::VertexBatch::AttribColor,"color");
				builder.addAttrib(ww::gfx::VertexBatch::AttribTexCoord0,"texcoord0");
				builder.addUniform("matrix");
				return builder.generate();
			}
			return NULL;
		}
		ShaderBuilder::ShaderBuilder()
		{
			hasFragmentShader = false;
			hasVertexShader = false;
			loadedFragmentShader = false;
			loadedVertexShader = false;
			fragSource = NULL;
			vertSource = NULL;
		}
		void ShaderBuilder::reset()
		{
			hasFragmentShader = false;
			hasVertexShader = false;
			loadedFragmentShader = false;
			loadedVertexShader = false;
			fragSource = NULL;
			vertSource = NULL;
		}
		bool ShaderBuilder::loadVertexShader(const char *fname, const char *source)
		{
			if (fname == NULL)
			{
				vertSource = (char*)source;
				hasVertexShader = true;
			}
			else
			{
				vertSource = file_alloc_contents(fname);
				if (vertSource == NULL)
				{
					printf("Failed to load vertex shader: %s\n",fname);
					return false;
				}
				loadedVertexShader = true;
				hasVertexShader = true;
			}
			return true;
		}
		bool ShaderBuilder::loadFragmentShader(const char *fname, const char *source)
		{
			if (fname == NULL)
			{
				fragSource = (char*)source;
				hasFragmentShader = true;
			}
			else
			{
				fragSource = file_alloc_contents(fname);
				if (fragSource == NULL)
				{
					printf("Failed to load fragment shader: %s\n",fname);
					return false;
				}
				loadedFragmentShader = true;
				hasFragmentShader = true;
			}
			return true;
		}

		Shader *ShaderBuilder::generate()
		{
			if (!ww::gfx::supportsOpenGL2())
			{
				printf("Shaders require OpenGL 2.0 or higher to run. Currently running %f\n",atof((char*)glGetString(GL_VERSION)));
				return NULL;
			}
			GLuint vertShader = 0, fragShader = 0;
			GLuint prog = glCreateProgram();
			if (hasVertexShader)
			{
				if (!compileShader(&vertShader,GL_VERTEX_SHADER,vertSource))
				{
					printf("Vertex shader failed to compile.\n");
					return NULL;
				}
			}
			if (hasFragmentShader)
			{
				if (!compileShader(&fragShader,GL_FRAGMENT_SHADER,fragSource))
				{
					printf("Fragment shader failed to compile.\n");
					return NULL;
				}
			}
			glAttachShader(prog, vertShader);
			glAttachShader(prog, fragShader);
			for(int i=0;i<attribNames.size();i++)
				glBindAttribLocation(prog, attribIDs.at(i), attribNames.at(i));
			attribNames.clear();
			attribIDs.clear();
			if (!linkProgram(prog))
			{
				printf("\n");
				glDeleteShader(vertShader);
				glDeleteShader(fragShader);
				glDeleteProgram(prog);
				return NULL;
			}
			// EVERYTHING OKAY SO FAR
			if (vertShader) {
				glDetachShader(prog, vertShader);
				glDeleteShader(vertShader);
			}
			if (fragShader) {
				glDetachShader(prog, fragShader);
				glDeleteShader(fragShader);
			}
			Shader *newshader = new Shader();
			
			newshader->builderSetProgram(prog);
			for(int i=0;i<uniforms.size();i++)
				newshader->builderAddUniform(uniforms.at(i));
			if (loadedVertexShader)
				delete vertSource;
			if (loadedFragmentShader)
				delete fragSource;
			vertSource = NULL;
			fragSource = NULL;
			return newshader;
		}
		void ShaderBuilder::addAttrib(GLuint attribID, const GLchar *attribName)
		{
			attribIDs.push_back(attribID);
			attribNames.push_back(attribName);
		}
		void ShaderBuilder::addUniform(const GLchar *uniformName)
		{
			uniforms.push_back(uniformName);
		}
		bool ShaderBuilder::linkProgram(GLuint program)
		{
			GLint status;
			glLinkProgram(program);
			glGetProgramiv(program, GL_LINK_STATUS, &status);
			if (status == 0)
			{
				GLint logLength;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
				if (logLength > 0)
				{
					GLchar *log = (GLchar *)malloc(logLength);
					glGetProgramInfoLog(program, logLength, &logLength, log);
					printf("Program link log:\n%s", log);
					free(log);
				}
				return false;
			}
			return true;
		}
		bool ShaderBuilder::compileShader(GLuint *shader, GLenum type, char *source)
		{
			GLint status = 0;
			int len = strlen(source);
			*shader = glCreateShader(type);
			glShaderSource(*shader, 1, (const GLchar**)&source, &len);
			glCompileShader(*shader);
			glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
			if (status == 0)
			{
				GLint logLength;
				glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
				if (logLength > 0)
				{
					GLchar *log = (GLchar *)malloc(logLength);
					glGetShaderInfoLog(*shader, logLength, &logLength, log);
					printf("Shader compile log:\n%s", log);
					free(log);
				}
				glDeleteShader(*shader);
				return false;
			}
			return true;
		}
	} // namespace gfx
} // namespace ww


