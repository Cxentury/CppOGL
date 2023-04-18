#ifndef CPP_OGL_SSAO_HEADER
#define CPP_OGL_SSAO_HEADER

#include "framebuffer.h"
#include "model.h"

class SSAO
{
public:
	SSAO(int width, int height);
	float lerp(float a, float b, float f);
	void createKernel(int length);
	void genTexturesAndFB(int width, int height, int length);
	void draw(Model& model);
private:
	Framebuffer gBuffer;
	unsigned int noiseTexture, colorBuffer, frameBuffer;
};

#endif // !CPP_OGL_SSAO_HEADER