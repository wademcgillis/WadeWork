#include <WadeWork/sfx.h>
#include <vector>
namespace ww
{
	namespace sfx
	{
		ALCdevice *openal_device;
		ALCcontext *openal_context;

		namespace ss
		{
			std::vector<Sound> sounds;
			std::vector<std::string> soundname;
			double scalefactor = 1;

			std::string ss_getname(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return "";
				return soundname.at(num);
			}
			
			double setsoundscalefactor(double factor)
			{
				if (factor > 0)
					scalefactor = factor;
				else
					scalefactor = 1;
				return 1;
			}
			double loadsound(std::string fname)
			{
				ww::sfx::Sound snd;
				if (!snd.load((char*)fname.c_str()))
					return -1;
				sounds.push_back(snd);
				setvol(sounds.size()-1,10000*scalefactor);
				return sounds.size()-1;
			}
			double loadmusic(std::string fname)
			{
				return -1;
			}
			double playsound(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				sounds.at(num).play();
				return 1;
			}
			double isplaying(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				return sounds.at(num).isPlaying();
			}
			double loopsound(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				sounds.at(num).loop();
				return 1;
			}
			double islooping(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				return sounds.at(num).isLooping();
			}
			double stopsound(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				sounds.at(num).stop();
				return 1;
			}
			double setvol(double soundid, double volume)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				if (volume < 0)
					volume = 0;
				sounds.at(num).setVolume(volume/10000.f);
				return 1;
			}
			double getvol(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				return sounds.at(num).getVolume() * 10000;
			}
			double setfreqAL(double soundid, double freq)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				sounds.at(num).setFreq(freq);
				return 1;
			}
			double setfreq(double soundid, double freq)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				sounds.at(num).setFreq(freq/44100.f);
				return 1;
			}
			double getfreq(double soundid)
			{
				int num = (int) soundid;
				if (num < 0 || num >= sounds.size())
					return -1;
				return 44100*sounds.at(num).getFreq();
			}
			double stopall()
			{
				unsigned int n = sounds.size();
				for(unsigned int i=0;i<n;i++)
					stopsound(i);
				return 1;
			}
			double getnextsoundid()
			{
				return 0;
			}
		} // namespace ss
	} // namespace sfx
} // namespace ww