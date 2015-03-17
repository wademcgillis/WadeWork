#include <WadeWork/Sound.h>
#define STB_VORBIS_HEADER_ONLY
#include <WadeWork/ext/stb_vorbis/stb_vorbis.c>
#include <map>
namespace ww
{
	namespace sfx
	{
		class ALBuffer;
		std::map<std::string, ALBuffer*> HugeBufferMap;

		class ALBuffer
		{
		private:
			ALuint buffer;
			short *data;
			stb_vorbis *vorb;
		public:
			ALBuffer()
			{
				buffer = 0;
				data = NULL;
				vorb = NULL;
			}
			~ALBuffer()
			{
				alDeleteBuffers(1, &buffer);
				stb_vorbis_close(vorb);
				delete data;
			}
			bool load(std::string fname)
			{
				// If we find the filename in the list of buffers previously loaded...
				if (HugeBufferMap.find(std::string(fname)) != HugeBufferMap.end())
				{
					// say that we already found it and return
					buffer = 0;
					return true;
				}
				else // otherwise add this filename to the huge buffer list!
					HugeBufferMap.insert(std::pair<std::string,ALBuffer*>(std::string(fname),this));
				
				int err = 0;
				vorb = stb_vorbis_open_filename((char*)fname.c_str(), &err, NULL);
				// Load a file and check to see if an error occurred
				if (err > 1)
				{
					printf("failed to open OGG: %s\n",fname.c_str());
					return false;
				}
				// do stb file loading magic. MAGIC.
				stb_vorbis_info info = stb_vorbis_get_info(vorb);
				unsigned int number_of_samples = stb_vorbis_stream_length_in_samples(vorb) * info.channels;
				unsigned int number_of_shorts = number_of_samples * sizeof(ALshort);
				// allow memory
				data = new short[number_of_samples];
				// write decompressed sound data into memory
				stb_vorbis_get_samples_short_interleaved(vorb, info.channels, data, number_of_shorts);
				// Generate the OpenAL buffer and insert the loaded data into it.
				alGenBuffers((ALuint)1, &buffer);
				alBufferData(buffer, (info.channels == 1)?AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, number_of_shorts, info.sample_rate);
				return true;
			}
			ALuint getBuffer()
			{
				return buffer;
			}
		};

		void Sound::gen(void *data, unsigned int count, unsigned int rate, bool SixteenBit)
		{
			ALuint buffer;
			alGenBuffers((ALuint)1, &buffer);
			alBufferData(buffer, (SixteenBit)?AL_FORMAT_MONO16:AL_FORMAT_MONO8, data, count, rate);
			alGenSources((ALuint)1, &source);
			alSourcef(source, AL_PITCH, 1.f);
			alSourcef(source, AL_GAIN, 3.f);
			alSource3f(source, AL_POSITION, 0, 0, 0);
			alSource3f(source, AL_VELOCITY, 0, 0, 0);
			alSourcei(source, AL_LOOPING, AL_FALSE);
			alSourcei(source, AL_BUFFER, buffer);
		}


		bool Sound::load(const char *path)
		{
			std::string filename = std::string(path);
			ALuint buffer;
			bool returnValue = true;
			// Check to see if the filename is in the magic big buffer list
			if (HugeBufferMap.find(std::string(filename)) != HugeBufferMap.end())
			{
				// if it is, we load the buffer from 
				buffer = (HugeBufferMap.find(std::string(filename)))->second->getBuffer();
				printf("OLD BUFFER: %s\n",filename.c_str());
			}
			else
			{
				ALBuffer *buffoon = new ALBuffer();
				returnValue = buffoon->load((std::string(filename)).c_str());
				if (!returnValue)
					printf("An error occurred while trying to load %s\n",filename);
				buffer = buffoon->getBuffer();
				HugeBufferMap.insert(std::pair<std::string,ALBuffer*>(std::string(filename),buffoon));
				printf("NEW BUFFER: %s\n",filename.c_str());
			}
			// Generate the OpenAL source and set all properties to inital values.
			alGenSources((ALuint)1, &source);
			alSourcef(source, AL_PITCH, 1.f);
			alSourcef(source, AL_GAIN, 1.f);
			alSource3f(source, AL_POSITION, 0, 0, 0);
			alSource3f(source, AL_VELOCITY, 0, 0, 0);
			alSourcei(source, AL_LOOPING, AL_FALSE);
			alSourcei(source, AL_BUFFER, buffer);
			return returnValue;
		}
		void Sound::play()
		{
			//printf("play\n");
			// set looping to false and play the sound
			alSourcei(source, AL_LOOPING, AL_FALSE);
			alSourcePlay(source);
		}
		bool Sound::isPlaying()
		{
			ALint playing = 0;
			alGetSourcei(source, AL_SOURCE_STATE, &playing);
			return (playing == AL_PLAYING);
		}
		void Sound::loop()
		{
			// set looping to true and play the sound
			alSourcei(source, AL_LOOPING, AL_FALSE);
			alSourcePlay(source);
		}
		bool Sound::isLooping()
		{
			ALint playing = 0;
			alGetSourcei(source, AL_SOURCE_STATE, &playing);
			if (playing == AL_PLAYING)
			{
				ALint looping = 0;
				alGetSourcei(source, AL_LOOPING, &looping);
				return (looping == AL_TRUE);
			}
			return false;
		}
		void Sound::stop()
		{
			alSourceStop(source);
		}
		void Sound::setVolume(float volume)
		{
			alSourcef(source, AL_GAIN, volume);
		}
		float Sound::getVolume()
		{
			ALfloat volume = 0;
			alGetSourcef(source, AL_GAIN, &volume);
			return volume;
		}
		ALuint Sound::getSource()
		{
			return source;
		}
		void Sound::setFreq(float freq)
		{
			alSourcef(source, AL_PITCH, freq);
		}
		float Sound::getFreq()
		{
			ALfloat freq = 0;
			alGetSourcef(source, AL_PITCH, &freq);
			return freq;
		}
	} // namespace sfx
} // namespace ww