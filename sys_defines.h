#ifndef __WadeWork__sys_defines_h__
#define __WadeWork__sys_defines_h__

// Individual Apple Platform Preprocessor Defines
#define PLATFORM_IOS (defined (__IPHONE_OS_VERSION_MIN_REQUIRED))
#define PLATFORM_OSX (defined (__APPLE__) && !PLATFORM_IOS)
// Apple Platform Preprocessor Define
#define PLATFORM_APPLE (defined (__APPLE__))
// Individual Apple Platform Preprocessor Defines
#define PLATFORM_WINDOWS (defined (_WIN32))
// Microsoft Platform Preprocessor Define
#define PLATFORM_MICROSOFT (defined (_WIN32))
// Personal Computer Preprocessor Define
#define PLATFORM_PC !PLATFORM_IOS
// Android?
#define PLATFORM_ANDROID defined(__ANDROID__)

#if !PLATFORM_APPLE
typedef int GLint;
typedef char GLchar;
#endif
#if PLATFORM_WINDOWS
#define NOMINMAX
#include <winsock2.h> // <-------- GRRR!
//#define WIN32_LEAN_AND_MEAN
//#define _WINSOCK2API_
#include <windows.h>
#endif
#if PLATFORM_APPLE
#include <Foundation/Foundation.h>
#endif
#include <stdio.h>
#include <string>
#endif