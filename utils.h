#ifndef __WadeWork__utils_h__
#define __WadeWork__utils_h__
namespace ww
{
	namespace utils
	{
		extern unsigned char *load_image(const char *path, int *width, int *height);

		char *file_alloc_contents(char *path, unsigned int *length = 0);

		namespace random
		{
			extern void seed(unsigned int n);
			extern unsigned int randomi(int n);
			extern float randomf(float f);
		} // namespace random
	} // namespace utils
} // namespace ww
#endif