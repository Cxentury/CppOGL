#include "headers/cubemap.h"

CubeMap::CubeMap(std::vector<std::string> paths) {
	this->m_texture = Texture::loadCubemap(paths);
	this->configureMesh();
    this->shader = std::make_unique<Shader>("shaders/cubemap.vert","shaders/cubemap.frag");
}

CubeMap::CubeMap(std::string dir, std::vector<std::string> paths){
	for(std::string& path : paths)
	{
		path = dir + "/" + path;
	}
	this->m_texture = Texture::loadCubemap(paths);
    this->shader = std::make_unique<Shader>("shaders/cubemap.vert","shaders/cubemap.frag");
	this->configureMesh();
}

CubeMap::CubeMap(std::string path) {
    this->m_texture = Texture::loadCubemapHDR(path);
    this->shader = std::make_unique<Shader>("shaders/cubemap.vert", "shaders/cubemap.frag");
    this->hdr = true;
    this->configureMesh();
}

void CubeMap::configureMesh() {
    this->m_vertices = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
    };
	this->configureBuffers();
}

void CubeMap::configureBuffers() {

	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &this->m_VBO);
	
	glBindVertexArray(this->m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_vertices.size(), &this->m_vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

	glBindVertexArray(0);
}

void CubeMap::draw(glm::mat4 view, glm::mat4 projection) {
    
    //do not write the the depth buffer
    glDepthMask(false);

    this->shader->use();
    this->shader->setMatrix4("view", glm::mat4(glm::mat3(view)));
    this->shader->setMatrix4("projection", projection);
    this->shader->setFloat("hdr", hdr);

    GLenum textureType = GL_TEXTURE_CUBE_MAP;
    /*this->shader->setFloat("skybox", 0);
    if (hdr) {
        glActiveTexture(GL_TEXTURE1);
        this->shader->setFloat("skyboxHDR", 1);
        textureType = GL_TEXTURE_2D;
    }
    else
        glActiveTexture(GL_TEXTURE0);
    */

    glBindVertexArray(this->m_VAO);
    textureType = GL_TEXTURE_2D;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);
    
    //glActiveTexture(GL_TEXTURE1);
    glBindTexture(textureType, this->m_texture.getID());

    glDrawArrays(GL_TRIANGLES, 0, this->m_vertices.size());
    
    
    glBindTexture(textureType, 0);
    glActiveTexture(GL_TEXTURE0);
    glDepthMask(true);
}