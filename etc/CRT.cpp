#include <WadeWork/etc/CRT.h>
namespace ww
{
	namespace etc
	{
		CRT::CRT()
		{
			printf("CRT::CRT()\n");
			srcWidth = 0;
			srcHeight = 0;
			scanlineTexture = NULL;
			initialized = false;
		}
		CRT::CRT(unsigned int srcWidth, unsigned int srcHeight, bool srcInterpolate, ww::gfx::Texture *scanlineTexture, float scanlineXrepeat, float scanlineYrepeat)
		{
			printf("CRT::CRT(...)\n");
			initialized = false;
			initialize(srcWidth, srcHeight, srcInterpolate, scanlineTexture, scanlineXrepeat, scanlineYrepeat);
		}

		const float CRT_MAX = 429.0/448.0;
		const float CRT_MIN = 400.0/448.0;

		#define sign(x) ((x)<0.f)?-1.f:1.f

		inline float FUNC(float n)
		{
			return CRT_MAX - (1.f-CRT_MIN) * n * n;
		}

		inline float INV_FUNC(float n)
		{
			return CRT_MAX - (CRT_MAX - CRT_MIN)/(CRT_MIN * CRT_MIN) * n * n;
		}


		glm::vec2 crtcoord(float x, float y)
		{
			glm::vec2 intex = glm::vec2(x,y);
			x = 2*x - 1;
			y = 2*y - 1;
			intex.x = x*INV_FUNC(y);
			intex.y = y*INV_FUNC(x);
			intex.x = (intex.x + 1.f)/2.f;
			intex.y = (intex.y + 1.f)/2.f;
			return intex;
		}

		glm::vec2 inv_crtcoord(float x, float y)
		{
			glm::vec2 screencoords = glm::vec2(x,y);
			screencoords *= 2.0f;
			screencoords.x -= 1.f;
			screencoords.y -= 1.f;
			glm::vec2 out = glm::vec2(0.f,0.f);
			out.x = screencoords.x/FUNC(screencoords.y);
			out.y = screencoords.y/FUNC(screencoords.x);
			if (abs(out.x) > 1)
				return glm::vec2(-1,-1);
			if (abs(out.y) > 1)
				return glm::vec2(-1,-1);
			out.x = (out.x + 1.f)/2.f;
			out.y = (out.y + 1.f)/2.f;
			return out;
		}



		bool CRT::initialize(unsigned int srcWidth, unsigned int srcHeight, bool srcInterpolate, ww::gfx::Texture *scanlineTexture, float scanlineXrepeat, float scanlineYrepeat)
		{
			if (initialized)
				return true;
			initialized = true;
			this->srcWidth = srcWidth;
			this->srcHeight = srcHeight;
			this->scanlineTexture = scanlineTexture;

			defaultShader = ww::gfx::ShaderBuilder::genericShader(ww::gfx::ShaderBuilder::DEFAULT_SHADER_NOLIGHTING);
			defaultMatrixID = defaultShader->getUniformIDFromName("matrix");

			ww::gfx::ShaderBuilder builder;

			const char *DEFAULT_VERTSHADER = "//#version 120\r\nattribute vec4 position;\r\nattribute vec4 color;\r\nattribute vec2 texcoord0;\r\nuniform mat4 matrix;\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nvoid main()\r\n{\r\n\tvertexColor = color;\r\n\ttexcoord0frag = texcoord0;\r\n\tgl_Position = matrix * position;\r\n}\0";

			const char *HBLUR_FRAGSHADER = "//#version 120\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nuniform sampler2D texture;\r\nuniform vec2 texSize;\r\nconst float N = .6;\r\nconst float N_0 = pow(N,0.0);\r\nconst float N_1 = pow(N,1.0);\r\nconst float N_2 = pow(N,2.0);\r\nconst float N_3 = pow(N,3.0);\r\nconst float N_4 = pow(N,4.0);\r\nconst float N_5 = pow(N,5.0);\r\nconst float N_6 = pow(N,6.0);\r\nvoid main()\r\n{	\r\n	vec4 at = texture2D(texture,texcoord0frag);\r\n	gl_FragColor = vec4(vertexColor.rgb * (\r\n	+ N_6 * texture2D(texture,vec2(clamp(texcoord0frag.x-6.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_5 * texture2D(texture,vec2(clamp(texcoord0frag.x-5.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_4 * texture2D(texture,vec2(clamp(texcoord0frag.x-4.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_3 * texture2D(texture,vec2(clamp(texcoord0frag.x-3.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_2 * texture2D(texture,vec2(clamp(texcoord0frag.x-2.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_1 * texture2D(texture,vec2(clamp(texcoord0frag.x-1.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ .75 * texture2D(texture,texcoord0frag)\r\n	+ N_1 * texture2D(texture,vec2(clamp(texcoord0frag.x+1.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_2 * texture2D(texture,vec2(clamp(texcoord0frag.x+2.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_3 * texture2D(texture,vec2(clamp(texcoord0frag.x+3.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_4 * texture2D(texture,vec2(clamp(texcoord0frag.x+4.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_5 * texture2D(texture,vec2(clamp(texcoord0frag.x+5.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	+ N_6 * texture2D(texture,vec2(clamp(texcoord0frag.x+6.0/texSize.x,0.5/texSize.x,1-0.5/texSize.x),texcoord0frag.y))\r\n	).rgb/2.0, 1.0);\r\n}\0";
			const char *VBLUR_FRAGSHADER = "//#version 120\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nuniform sampler2D texture;\r\nuniform vec2 texSize;\r\nconst float N = .6;\r\nconst float N_0 = pow(N,0.0);\r\nconst float N_1 = pow(N,1.0);\r\nconst float N_2 = pow(N,2.0);\r\nconst float N_3 = pow(N,3.0);\r\nconst float N_4 = pow(N,4.0);\r\nconst float N_5 = pow(N,5.0);\r\nconst float N_6 = pow(N,6.0);\r\nvoid main()\r\n{	\r\n	vec4 at = texture2D(texture,texcoord0frag);\r\n	gl_FragColor = vec4(vertexColor.rgb * (\r\n	+ N_6 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-6.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_5 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-5.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_4 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-4.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_3 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-3.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_2 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-2.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_1 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y-1.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ .75 * texture2D(texture,texcoord0frag)\r\n	+ N_1 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+1.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_2 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+2.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_3 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+3.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_4 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+4.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_5 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+5.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	+ N_6 * texture2D(texture,vec2(texcoord0frag.x,clamp(texcoord0frag.y+6.0/texSize.y,0.5/texSize.y,1-0.5/texSize.y)))\r\n	).rgb/2.0, 1.0);\r\n}\0";
			const char *LIGHTEN_FRAGSHADER = "//#version 120\r\nvarying vec4 vertexColor;\r\nvarying vec2 texcoord0frag;\r\nuniform sampler2D texture;\r\n\r\nvoid main()\r\n{\r\n	vec4 col = texture2D(texture,texcoord0frag);\r\n	col.r = .05+col.r * .95;\r\n	col.g = .05+col.g * .95;\r\n	col.b = .05+col.b * .95;\r\n	gl_FragColor = vertexColor * col;//vec4(vertexColor.rgb * col.rgb, .5);\r\n}\0";

			builder.reset();
				builder.loadFragmentShader(NULL,HBLUR_FRAGSHADER);
				builder.loadVertexShader(NULL,DEFAULT_VERTSHADER);
				builder.addAttrib(ww::gfx::VertexBatch::AttribPosition,"position");
				builder.addAttrib(ww::gfx::VertexBatch::AttribColor,"color");
				builder.addAttrib(ww::gfx::VertexBatch::AttribTexCoord0,"texcoord0");
				builder.addUniform("matrix");
				builder.addUniform("texSize");
			hBlurShader = builder.generate();
			hBlurTexSizeID = hBlurShader->getUniformIDFromName("texSize");
			hBlurMatrixID = hBlurShader->getUniformIDFromName("matrix");

			builder.reset();
				builder.loadFragmentShader(NULL,VBLUR_FRAGSHADER);
				builder.loadVertexShader(NULL,DEFAULT_VERTSHADER);
				builder.addAttrib(ww::gfx::VertexBatch::AttribPosition,"position");
				builder.addAttrib(ww::gfx::VertexBatch::AttribColor,"color");
				builder.addAttrib(ww::gfx::VertexBatch::AttribTexCoord0,"texcoord0");
				builder.addUniform("matrix");
				builder.addUniform("texSize");
			vBlurShader = builder.generate();
			vBlurTexSizeID = vBlurShader->getUniformIDFromName("texSize");
			vBlurMatrixID = vBlurShader->getUniformIDFromName("matrix");

			builder.reset();
				builder.loadFragmentShader(NULL,LIGHTEN_FRAGSHADER);
				builder.loadVertexShader(NULL,DEFAULT_VERTSHADER);
				builder.addAttrib(ww::gfx::VertexBatch::AttribPosition,"position");
				builder.addAttrib(ww::gfx::VertexBatch::AttribColor,"color");
				builder.addAttrib(ww::gfx::VertexBatch::AttribTexCoord0,"texcoord0");
				builder.addUniform("matrix");
			lightenShader = builder.generate();
			lightenMatrixID = lightenShader->getUniformIDFromName("matrix");	

			int W, H;

			printf("SOURCE: %i, %i\n",srcWidth,srcHeight);
			W = (int)pow(2.f,(float)((int)(log((float)srcWidth)/log(2.f))));
			if (W < srcWidth)
				W *= 2;
			H = (int)pow(2.f,(float)((int)(log((float)srcHeight)/log(2.f))));
			if (H < srcHeight)
				H *= 2;
			printf("TEXTURE: %i, %i\n",W,H);

			SRC_XRATIO = (float)srcWidth/(float)W;
			SRC_YRATIO = (float)srcHeight/(float)H;

			SOURCE = new ww::gfx::RenderTarget(W,H);
			SOURCE->getTexture()->setLinearInterpolation(srcInterpolate);

			FINAL = new ww::gfx::RenderTarget(W,H);
			FINAL->getTexture()->setLinearInterpolation(false);

			for(unsigned int i = 0;i < BLOOM_COUNT; i++)
			{
				W = (int)pow(2.f,(float)((int)(log((float)srcWidth/(float)pow(2.f,(float)i))/log(2.f))));
				if (W < srcWidth/(int)pow(2.f,(float)i))
					W *= 2;
				H = (int)pow(2.f,(float)((int)(log((float)srcHeight/(float)pow(2.f,(float)i))/log(2.f))));
				if (H < srcHeight/(int)pow(2.f,(float)i))
					H *= 2;
				printf("OUTPUT[%i] (%i, %i)\n",i,W,H);
				OUTPUT1[i] = new ww::gfx::RenderTarget(W,H);
				OUTPUT1[i]->getTexture()->setLinearInterpolation(i > 0);
				OUTPUT2[i] = new ww::gfx::RenderTarget(W,H);
				OUTPUT2[i]->getTexture()->setLinearInterpolation(i > 0);
			}

			const unsigned int RESOLUTION = 15;
			const float INV_RESOLUTION = 1.f/(float)RESOLUTION;

			float XR = 0.f;
			float YR = 0.f;
			XR = SRC_XRATIO;
			YR = SRC_YRATIO;

			crtBatch.clear();
			for(int in_x = 0; in_x < RESOLUTION; in_x++)
				for(int in_y = 0; in_y < RESOLUTION; in_y++)
				{
					glm::vec2 pos = glm::vec2(0.f,0.f);
				
					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 1,0
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR,	(in_y+0)*INV_RESOLUTION*YR));

					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 1,1
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR,	(in_y+1)*INV_RESOLUTION*YR));
				
					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 0,0
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+0)*INV_RESOLUTION*YR));
				
					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 0,0
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+0)*INV_RESOLUTION*YR));

					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 1,1
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR, (in_y+1)*INV_RESOLUTION*YR));
					

					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 0,1
					crtBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+1)*INV_RESOLUTION*YR));
				}
			crtBatch.update();

			XR = scanlineXrepeat;//(float)scanlineTextureSubwidth/(float)scanlineTexture->getWidth();
			YR = scanlineYrepeat;//(float)scanlineTextureSubwidth/(float)scanlineTexture->getHeight();
			scanlineBatch.clear();
			for(int in_x = 0; in_x < RESOLUTION; in_x++)
				for(int in_y = 0; in_y < RESOLUTION; in_y++)
				{
					glm::vec2 pos = glm::vec2(0.f,0.f);
				
					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 1,0
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR,	(in_y+0)*INV_RESOLUTION*YR));

					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 1,1
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR,	(in_y+1)*INV_RESOLUTION*YR));
				
					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 0,0
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+0)*INV_RESOLUTION*YR));
				
					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+0)*INV_RESOLUTION); // 0,0
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+0)*INV_RESOLUTION*YR));

					pos = crtcoord((in_x+1)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 1,1
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	(in_x+1)*INV_RESOLUTION*XR, (in_y+1)*INV_RESOLUTION*YR));
					

					pos = crtcoord((in_x+0)*INV_RESOLUTION,(in_y+1)*INV_RESOLUTION); // 0,1
					scanlineBatch.pushvertex(ww::gfx::MakeVertex(pos.x,pos.y,0,0xFFFFFFFF,	in_x*INV_RESOLUTION*XR,		(in_y+1)*INV_RESOLUTION*YR));
				}
			scanlineBatch.update();

			float x1, y1, x2, y2;

			x1 = .5f/(float)SOURCE->getWidth();
			x2 = ((float)srcWidth-.5f)/(float)SOURCE->getWidth();
			y1 = 0.5f/(float)SOURCE->getHeight();
			y2 = ((float)srcHeight-.5f)/(float)SOURCE->getHeight();

			batchBACK.clear();
				batchBACK.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,	x2,	y1));
				batchBACK.pushvertex(ww::gfx::MakeVertex(1,0,0,0xFFFFFFFF,	x2,	y2));
				batchBACK.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,	x1,	y2));
				batchBACK.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,	x2,	y1));
				batchBACK.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,	x1,	y2));
				batchBACK.pushvertex(ww::gfx::MakeVertex(0,1,0,0xFFFFFFFF,	x1,	y1));
			batchBACK.update();

			batchBACK_ALPHA.clear();
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(1,1,0,0x20FFFFFF,	x2,	y2));
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(1,0,0,0x20FFFFFF,	x2,	y1));
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(0,0,0,0x20FFFFFF,	x1,	y1));
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(1,1,0,0x20FFFFFF,	x2,	y2));
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(0,0,0,0x20FFFFFF,	x1,	y1));
				batchBACK_ALPHA.pushvertex(ww::gfx::MakeVertex(0,1,0,0x20FFFFFF,	x1,	y2));
			batchBACK_ALPHA.update();

			/*x1 = .5f/(float)scanlineTexture->getWidth();
			x2 = ((float)scanlineTextureSubwidth-.5f)/(float)scanlineTexture->getWidth();
			y1 = .5f/(float)scanlineTexture->getHeight();
			y2 = ((float)scanlineTextureSubheight-.5f)/(float)scanlineTexture->getHeight();

			scanlineX = scanlineTextureSubwidth/(float)scanlineTexture->getWidth();
			scanlineY = scanlineTextureSubheight/(float)scanlineTexture->getHeight();

			batchScanlines.clear();
				batchScanlines.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,	x2,	y1));
				batchScanlines.pushvertex(ww::gfx::MakeVertex(1,0,0,0xFFFFFFFF,	x2,	y2));
				batchScanlines.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,	x1,	y2));
				batchScanlines.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,	x2,	y1));
				batchScanlines.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,	x1,	y2));
				batchScanlines.pushvertex(ww::gfx::MakeVertex(0,1,0,0xFFFFFFFF,	x1,	y1));
			batchScanlines.update();*/
			return true;
		}
		void CRT::begin()
		{
			ww::gfx::setRenderTarget(SOURCE);
				glViewport(0,0,srcWidth,srcHeight);
				glScissor(0,0,srcWidth,srcHeight);
				glClearColor(1.f,0.f,0.f,1.f);
				glClear(GL_COLOR_BUFFER_BIT);
		}
		void CRT::end()
		{
			for(unsigned int i = 0; i < BLOOM_COUNT; i++)
			{
				ww::gfx::setRenderTarget(OUTPUT1[i]);
					glViewport(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glScissor(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glClearColor(0.f,0.f,0.f,0.f);
					glClear(GL_COLOR_BUFFER_BIT);
					defaultShader->makeActive();
					defaultShader->setMatrix(defaultMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,0.f,1.f,-1.f,1.f)));
					SOURCE->getTexture()->bind();
					batchBACK.draw();
				ww::gfx::setRenderTarget(OUTPUT2[i]);
					glViewport(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glScissor(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glClearColor(0.f,0.f,0.f,0.f);
					glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
					hBlurShader->makeActive();
					hBlurShader->setMatrix(vBlurMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,0.f,1.f,-1.f,1.f)));
					glUniform2f(hBlurTexSizeID,(float)OUTPUT1[i]->getWidth(),(float)OUTPUT1[i]->getHeight());
					OUTPUT1[i]->getTexture()->bind();
					batchBACK.draw();
				ww::gfx::setRenderTarget(OUTPUT1[i]);
					glViewport(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glScissor(0,0,OUTPUT1[i]->getWidth()*SRC_XRATIO,OUTPUT1[i]->getHeight()*SRC_YRATIO);
					glClearColor(0.f,0.f,0.f,0.f);
					glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);
					vBlurShader->makeActive();
					vBlurShader->setMatrix(vBlurMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,0.f,1.f,-1.f,1.f)));
					glUniform2f(vBlurTexSizeID,(float)OUTPUT1[i]->getWidth(),(float)OUTPUT1[i]->getHeight());
					OUTPUT2[i]->getTexture()->bind();
					batchBACK.draw();
			}
			ww::gfx::setRenderTarget(FINAL);
				glViewport(0,0,srcWidth,srcHeight);
				glScissor(0,0,srcWidth,srcHeight);
				glClearColor(0.f,0.f,1.f,1.f);
				glClear(GL_COLOR_BUFFER_BIT);

				lightenShader->makeActive();
				lightenShader->setMatrix(lightenMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,0.f,1.f,-1.f,1.f)));
				SOURCE->getTexture()->bind();
				batchBACK.draw();

				for(int i=0;i<BLOOM_COUNT;i++)
				{
					OUTPUT1[i]->getTexture()->bind();
					batchBACK_ALPHA.draw();
				}
			ww::gfx::resetRenderTarget();
		}
		void CRT::draw()
		{
			FINAL->getTexture()->bind();
			crtBatch.draw();
			scanlineTexture->bind();
			ww::gfx::setBlendMode(ww::gfx::BM_MULTIPLY);
			scanlineBatch.draw();
			ww::gfx::setBlendMode(ww::gfx::BM_NORMAL);
		}
	} // namespace etc
} // namespace ww