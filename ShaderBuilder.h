#ifndef __WadeWork__ShaderBuilder_h__
#define __WadeWork__ShaderBuilder_h__
#if PLATFORM_IOS
#include <UIKit/UIKit.h>
#endif
#if PLATFORM_APPLE
#import <Foundation/Foundation.h>
#endif
#include <vector>
#include <WadeWork/Shader.h>

namespace ww
{
	namespace gfx
	{
		class ShaderBuilder
		{
		private:
			std::vector<const char*> attribNames;
			std::vector<GLuint> attribIDs;
			
			std::vector<const char*> uniforms;
			
			char *vertSource;
			char *fragSource;
			bool loadedVertexShader;
			bool loadedFragmentShader;
			bool hasVertexShader;
			bool hasFragmentShader;
			
			bool compileShader(GLuint *shader, GLenum type, char *source);
			bool linkProgram(GLuint program);
		public:
			static const unsigned int DEFAULT_SHADER_NOLIGHTING = 0;
			ShaderBuilder();
			void reset();
			bool loadFragmentShader(const char *fname, const char *source = NULL);
			bool loadVertexShader(const char *fname, const char *source = NULL);
			void addAttrib(GLuint attribID, const GLchar *attribName);
			void addUniform(const GLchar *uniformName);
			Shader *generate();
			static Shader *genericShader(unsigned int shaderID);
		};
	}
}
#endif /* defined(__Wumbo__Wumbo_Shader__) */
