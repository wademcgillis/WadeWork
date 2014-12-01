#ifndef __WadeWork__WWSFX_h__
#define __WadeWork__WWSFX_h__
namespace ww
{
	namespace sfx
	{
		namespace priv
		{
			class WWSFX
			{
			public:
				void load(const char *path) {}
				void play() {}
				bool isPlaying() { return false; }
				void loop() {}
				bool isLooping() { return false; }
				void stop() {}
				void setVolume(float volume) {}
				float getVolume() { return 0; }
			};
		} // namespace priv
	} // namespace sfx
} // namespace ww
#endif