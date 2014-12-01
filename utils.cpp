#include <WadeWork/utils.h>
#include <climits>
#include <iostream>
namespace ww
{
	namespace utils
	{
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