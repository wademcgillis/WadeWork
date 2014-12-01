#ifndef __WadeWork__sfx_h__
#define __WadeWork__sfx_h__
#include <WadeWork/sys_defines.h>
#include <WadeWork/openal.h>
#include <WadeWork/Sound.h>
#include <WadeWork/BGM.h>
namespace ww
{
	namespace sfx
	{
		/*class BGM : public WWSFX
		{
		public:
			//ALuint getSource();
		};*/

		namespace ss
		{
			extern double unload();
			extern double setsoundscalefactor(double factor);
			extern double setvol(double soundid, double volume);
			extern double loadsound(std::string fname);
			extern double loadmusic(std::string fname);
			extern double playsound(double soundid);
			extern double isplaying(double soundid);
			extern double loopsound(double soundid);
			extern double islooping(double soundid);
			extern double stopsound(double soundid);
			extern double setvol(double soundid, double volume);
			extern double getvol(double soundid);
			extern double setfreqAL(double soundid, double freq);
			extern double setfreq(double soundid, double freq);
			extern double getfreq(double soundid);
			extern double stopall();
			extern double getnextsoundid();
			extern std::string ss_getname(double soundid);
		}
	};
};
#endif