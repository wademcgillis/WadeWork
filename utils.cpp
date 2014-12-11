#include <WadeWork/utils.h>
#include <climits>
#include <iostream>
#include <WadeWork/ext/stb_image/stb_image.h>

namespace ww
{
	namespace utils
	{
		unsigned char *load_image(const char *path, int *width, int *height)
		{
			return stbi_load(path,width,height,NULL,false);
		}

		char *file_alloc_contents(char *path)
		{
			// get contents of file
			FILE *f = fopen(path,"rb");
			if (f != NULL)
			{
				// if file loaded
				char *contents = NULL;
				int len;
				// seek to end
				fseek(f,0,SEEK_END);
				// get length
				len = ftell(f);
				// seek to beginning
				fseek(f,0,SEEK_SET);
				// new char array
				contents = new char[len+1];
				// set to zero
				memset(contents,0,len+1);
				// read it
				fread(contents, len, 1, f);
				// close file
				fclose(f);
				return contents;
			}
			else
				return NULL;
		}

		namespace random
		{
			inline unsigned int bitshiftWrapRight(unsigned int n, unsigned int shift)
			{
				return (n >> (shift%32)) | (n << (32-(shift%32)));
			}
			inline unsigned int bitshiftWrapLeft(unsigned int n, unsigned int shift)
			{
				return (n << (shift%32)) | (n >> (32-(shift%32)));
			}

			unsigned int R = 0;
			void seed(unsigned int n)
			{
				if (n)
					R = n;
				else
					R = rand();
			}
			unsigned int randomi(int n)
			{
				if (R == 0)
					return 0;
				//printf("R: %i ",R);
				R ^= (R << 21);
				R ^= bitshiftWrapRight(R,35);
				R ^= (R << 4);
				//printf("-> %i\n",R);
				/*r ^= r << 21;
				r ^= r >>> 35;
				r ^= r << 4;*/
				return R % n;
			}
			float randomf(float f)
			{
				if (R == 0)
					return 0;
				return f * (float)randomi(UINT_MAX)/(float)UINT_MAX;
			}
		} // namespace random
	} // namespace utils
} // namespace ww