#ifndef __WadeWork__utils_h__
#define __WadeWork__utils_h__
namespace ww
{
	namespace utils
	{
		namespace random
		{
			extern void seed(unsigned int n);
			extern unsigned int randomi(int n);
			extern float randomf(float f);
		} // namespace random
	} // namespace utils
} // namespace ww
#endif