#ifndef __CRT_h__
#define __CRT_h__
#include <WadeWork/gfx.h>
namespace ww
{
	namespace etc
	{
		class CRT
		{
		private:
			ww::gfx::RenderTarget *SOURCE;
			float SRC_XRATIO;
			float SRC_YRATIO;

			float SCAN_XREPEAT;
			float SCAN_YREPEAT;

			static const unsigned int BLOOM_COUNT = 3;
			ww::gfx::RenderTarget *OUTPUT1[BLOOM_COUNT];
			ww::gfx::RenderTarget *OUTPUT2[BLOOM_COUNT];
			ww::gfx::RenderTarget *FINAL;
			unsigned int srcWidth, srcHeight;

			ww::gfx::Shader *defaultShader;
			GLuint defaultMatrixID;
			
			ww::gfx::Shader *hBlurShader;
			GLuint hBlurMatrixID;
			GLuint hBlurTexSizeID;
			
			ww::gfx::Shader *vBlurShader;
			GLuint vBlurMatrixID;
			GLuint vBlurTexSizeID;

			ww::gfx::Shader *lightenShader;
			GLuint lightenMatrixID;

			bool initialized;
			ww::gfx::Texture *scanlineTexture;
			ww::gfx::VertexBatch batchBACK;
			ww::gfx::VertexBatch batchBACK_ALPHA;
			//ww::gfx::VertexBatch batchScanlines;

			ww::gfx::VertexBatch crtBatch;
			ww::gfx::VertexBatch scanlineBatch;
			float scanlineX, scanlineY;
		public:
			CRT();
			CRT::CRT(unsigned int srcWidth, unsigned int srcHeight, bool srcInterpolate, ww::gfx::Texture *scanlineTexture, float scanlineXrepeat, float scanlineYrepeat);
			bool initialize(unsigned int srcWidth, unsigned int srcHeight, bool srcInterpolate, ww::gfx::Texture *scanlineTexture, float scanlineXrepeat, float scanlineYrepeat);
			void begin();
			void end();
			void draw();
		};
	} // namespace etc
} // namespace ww
#endif