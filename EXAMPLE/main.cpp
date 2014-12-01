#include <WadeWork/wadework.h>
#include <time.h>

#define M_PI 3.1415926535f

bool doinit();
bool dorun();
bool doend();

const unsigned int window_width = 512;
const unsigned int window_height = 448;

int main(int argc, char *argv[])
{
	ww::gfx::setWindowSize(window_width,window_height);
	ww::sys::setInitCallback(doinit);
	ww::sys::setTimerCallback(dorun);
	ww::sys::setDeinitCallback(doend);
	ww::sys::setTimerResolution(60);
	//ww::setForegroundCallback(NULL);
	//ww::setBackgroundCallback(NULL);
#if 1
	return ww::sys::setup(ww::sys::CONFIG_OPENGL2 | ww::sys::CONFIG_DISABLE_OPENGL_DEPTHBUFFER);// | ww::sys::CONFIG_XINPUT);
#else
	ww::sys::setup(ww::sys::CONFIG_OPENGL2 | ww::sys::CONFIG_CUSTOM_TIMER | ww::sys::CONFIG_XINPUT | ww::sys::CONFIG_OPENAL);
	while(1)
	{
		ww::sys::customTimer_timerBegin();
		printf("calling!\n");
		if (ww::input::keyboard::isButtonDown(ww::input::key::A))
			printf("A!\n");
		printf("mouse x: %i\ty: %i\n",ww::input::mouse::getX(),ww::input::mouse::getY());
		ww::gfx::display();
		ww::sys::customTimer_timerFinish();
	}
	ww::sys::customTimer_unsetup();
#endif
}

ww::gfx::Shader *defaultShader = NULL;
GLint defaultVertexMatrixID;

#include <WadeWork/etc/CRT.h>
ww::etc::CRT *myCRT = NULL;

ww::gfx::Texture exampleTexture;
ww::gfx::Texture exampleScanlines;
ww::gfx::VertexBatch exampleBatch;

bool doinit()
{
	exampleTexture.load("example.png");
	exampleScanlines.load("example_scanline.png");

	defaultShader = ww::gfx::ShaderBuilder::genericShader(ww::gfx::ShaderBuilder::DEFAULT_SHADER_NOLIGHTING);
	if (defaultShader == NULL)
		defaultVertexMatrixID = defaultShader->getUniformIDFromName("matrix");

	myCRT = new ww::etc::CRT(256,224,false,&exampleScanlines,256/2,224/2);

	float x1, x2, y1, y2;
	x1 = 0.f;
	y1 = 0.f;
	x2 = 256.f / 256.f;
	y2 = 224.f / 256.f;
	exampleBatch.clear();
		exampleBatch.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,x2,y2));
		exampleBatch.pushvertex(ww::gfx::MakeVertex(1,0,0,0xFFFFFFFF,x2,y1));
		exampleBatch.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,x1,y1));
		exampleBatch.pushvertex(ww::gfx::MakeVertex(1,1,0,0xFFFFFFFF,x2,y2));
		exampleBatch.pushvertex(ww::gfx::MakeVertex(0,0,0,0xFFFFFFFF,x1,y1));
		exampleBatch.pushvertex(ww::gfx::MakeVertex(0,1,0,0xFFFFFFFF,x1,y2));
	exampleBatch.update();
	return true;
}

bool dorun()
{
	myCRT->begin();
		glViewport(0,0,256,224);
		glScissor(0,0,256,224);
		defaultShader->makeActive();
		defaultShader->setMatrix(defaultVertexMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,1.f,0.f,-1.f,1.f)));
		glClearColor(0.f,1.f,0.f,0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		exampleTexture.bind();
		exampleBatch.draw();
	myCRT->end();

	glViewport(0,0,window_width,window_height);
	glScissor(0,0,window_width,window_height);
	glClearColor(0.f,0.f,0.f,1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	defaultShader->makeActive();
	defaultShader->setMatrix(defaultVertexMatrixID,(float*)glm::value_ptr(glm::ortho(0.f,1.f,1.f,0.f,-1.f,1.f)));
	myCRT->draw();

	return true;
}

bool doend()
{
	//ww::terminate();
	printf("doend\n");
	return true;
}