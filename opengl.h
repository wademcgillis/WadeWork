#ifndef __WadeWork__opengl_h__
#define __WadeWork__opengl_h__
#include <WadeWork/sys_defines.h>
#if PLATFORM_WINDOWS
#include <GL/glew.h>
#include <GL/wglew.h>
#elif PLATFORM_IOS
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#elif PLATFORM_OSX
#import <OpenGL/gl.h>
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.inl>
#include <glm/gtc/type_ptr.hpp>

#endif