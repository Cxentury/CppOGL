#ifndef CPP_OGL_SSAO_HEADER
#define CPP_OGL_SSAO_HEADER

#include "framebuffer.h"
#include "model.h"
#include "gbuffer.h"

class SSAO
{
public:
	SSAO(int width, int height);
	float lerp(float a, float b, float f);
	void createKernel(int length);
	void genTexturesAndFB(int width, int height, int length);
	void draw(Framebuffer* gBuffer, Shader* shader, glm::mat4& projection);
private:
	Model quad{"models/postProcessing/quad.obj"};
	unsigned int noiseTexture, colorBuffer, frameBuffer;
	std::vector<glm::vec3> kernel;
};

#endif // !CPP_OGL_SSAO_HEADER