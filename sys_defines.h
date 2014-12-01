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

#if !PLATFORM_APPLE
typedef int GLint;
typedef char GLchar;
#endif
#if PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#if PLATFORM_APPLE
#include <Foundation/Foundation.h>
#endif
#include <stdio.h>
#include <string>
#endif