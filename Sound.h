#ifndef __WadeWork__Sound_h__
#define __WadeWork__Sound_h__
#include <WadeWork/openal.h>
#include <WadeWork/priv/WWSFX.h>
namespace ww
{
	namespace sfx
	{
		class Sound : public ww::sfx::priv::WWSFX
		{
		private:
			unsigned short _volume;
			bool _playing;
			bool _looping;
			ALuint source;
		public:
			void gen(void *data, unsigned int count, unsigned int rate, bool SixteenBit = false);
			bool load(const char *path);
			void play();
			bool isPlaying();
			void loop();
			bool isLooping();
			void stop();
			void setVolume(float volume);
			float getVolume();
			void setFreq(float freq);
			float getFreq();

			ALuint getSource();
		};
	} // namespace sfx
} // namespace ww
#endif