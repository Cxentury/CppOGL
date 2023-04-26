#include "headers/framebuffer.h"
#include "headers/logger.h"

Framebuffer::Framebuffer() {}

Framebuffer::Framebuffer(int width, int height, int nbAttachments, bool depthBuffer, GLenum mode, GLenum wrapping)
{
	this->m_width = width;
	this->m_height = height;
	this->m_depthBuffer = depthBuffer;

	glGenFramebuffers(1, &this->m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_ID);

	for (unsigned int i = 0; i < nbAttachments; i++) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		this->colorAttachements.push_back(textureID);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textureID, 0);
		this->attachmentEnum.push_back(GL_COLOR_ATTACHMENT0 + i);
		//Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	if (depthBuffer) {
		glGenTextures(1, &this->m_depthBufferID);
		glBindTexture(GL_TEXTURE_2D, this->m_depthBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//Unbind
		glBindTexture(GL_TEXTURE_2D, 0);
		//attach to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->m_depthBufferID, 0);
	}
	/*Stencil and depth*/
	else {
		glGenRenderbuffers(1, &this->m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, this->m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		//Unbind
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->m_rbo);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
		logger.log("Error in framebuffer");
		exit(-1);
	}

	//unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::use(int width, int height) {

	//We need to recreate the texture buffer if the dimension of the scene changes
	if (this->m_width != width || this->m_height != height) {
		
		for (unsigned int& id : this->colorAttachements) {
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

		}
		if (this->m_depthBuffer) {
			glBindTexture(GL_TEXTURE_2D, this->m_depthBufferID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		}
		else {
			glBindRenderbuffer(GL_RENDERBUFFER, this->m_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_ID);
	glDrawBuffers(this->attachmentEnum.size(), this->attachmentEnum.data());
}

void Framebuffer::setInputTextures(Shader* shader) {
	
	shader->use();

	unsigned int i;
	std::string name;

	for (i = 0; i < this->colorAttachements.size(); i++)
	{
		name = "texture" + std::to_string(i);
		shader->setInt(name,i);
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, this->colorAttachements[i]);

	}
	
	if (this->m_depthBuffer) {
		glActiveTexture(GL_TEXTURE0+i);
		shader->setInt("depthBufferTexture", i);
		glBindTexture(GL_TEXTURE_2D, this->m_depthBufferID);
	}
	
	//Reset to default
	glActiveTexture(GL_TEXTURE0);
}

int Framebuffer::getTextureID(int position) {
	return this->colorAttachements[position];
}