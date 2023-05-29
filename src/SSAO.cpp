#include <random>
#include "headers/SSAO.h"

SSAO::SSAO(int width, int height) {
	createKernel(64);
	genTexturesAndFB(width, height, 16);
}

void SSAO::createKernel(int length) {
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine engine;

	float scale;
	for (unsigned int i = 0; i < length; i++)
	{
		glm::vec3 sample(randomFloats(engine) * 2.0f - 1.0f, randomFloats(engine) * 2.0f - 1.0f, randomFloats(engine));
		sample = glm::normalize(sample);

		scale = (float)i / 64.0f;
		scale = lerp(0.1, 1.0, scale);
		sample *= scale;

		this->kernel.push_back(sample);
	}
}

float SSAO::lerp(float a, float b, float f) {
	return a + f * (b - a);
}

void SSAO::genTexturesAndFB(int width, int height, int length) {

	//Noise
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine engine;
	std::vector<glm::vec3> noise;

	for (unsigned int i = 0; i < length; i++)
	{
		glm::vec3 vecNoise(randomFloats(engine) * 2.0f - 1.0f, randomFloats(engine) * 2.0f - 1.0f, 0.0f);
		noise.push_back(vecNoise);
	}

	glGenTextures(1, &this->noiseTexture);
	glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RED, GL_FLOAT, &noise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//Framebuffer
	glGenFramebuffers(1, &this->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->frameBuffer);

	glGenTextures(1, &this->colorBuffer);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffer, 0);

}

void SSAO::draw(Framebuffer* gBuffer, Shader* shader, glm::mat4& projection) {

	shader->use();
	shader->setInt("texPosition", 0);
	shader->setInt("texNormal", 1);
	shader->setInt("texNoise", 2);

	//position
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getTextureID(1));

	//normal
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gBuffer->getTextureID(2));

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform3fv(glGetUniformLocation(shader->getId(), "samples"), this->kernel.size(), glm::value_ptr(this->kernel[0]));
	shader->setMatrix4("projection", projection);
	this->quad.draw(shader);
}