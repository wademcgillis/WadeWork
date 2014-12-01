#ifndef __WadeWork__openal_h__
#define __WadeWork__openal_h__
#include <WadeWork/sys_defines.h>
#if PLATFORM_WINDOWS
#include <AL/al.h>
#include <AL/alc.h>
#elif PLATFORM_OSX
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif PLATFORM_IOS
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
#endif