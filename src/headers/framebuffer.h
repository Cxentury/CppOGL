#ifndef CPP_OGL_FRAMEBUFFER_HEADER
#define CPP_OGL_FRAMEBUFFER_HEADER

#include <glad/glad.h>
#include <vector>
#include "shader.h"

class Framebuffer
{
public:
	/**
	 * @param width - Width of the textures in the framebuffer (usually screen width)
	 * @param height - Height of the texture in the framebuffer (usually screen height)
	 * @param nbAttachments - Number of textures needed in the framebuffer
	 * @param depthBuffer - True if the depthBuffer need to be read to
	 */
	Framebuffer(int width, int height, int nbAttachments, bool depthBuffer = false, GLenum mode = GL_LINEAR, GLenum wrapping = GL_REPEAT);

	Framebuffer();
	/**
	 * Add {size} texures as uniforms to the shader in parameter
	 *
	 * @param shader - The shader who will get the textures generated via the framebuffer
	 * @param size - Number of textures to be added to the shader
	 */
	void setInputTextures(Shader* shader);

	/**
	 * Binds the frameBuffer and checks if the textures need to be redimensioned
	 * (by comparing the current width and height with the ones in parameter)
	 *
	 * @param width - The new width of the framebuffer
	 * @param height - The new height of the framebuffer
	 */
	void use(int width, int height);

	int getTextureID(int position);

private:
	// IDs if the textures
	std::vector<unsigned int> colorAttachements;
	std::vector<GLenum> attachmentEnum;

protected:
	unsigned int m_ID, m_rbo, m_depthBufferID;
	// Current widht and height of the textures
	int m_width, m_height;
	// This bool is true if the depth buffer needs to be read
	bool m_depthBuffer;
};

#endif // !CPP_OGL_FRAMEBUFFER_HEADER