#ifndef MODEL_MATERIAL_HEADER
#define MODEL_MATERIAL_HEADER
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

enum mat_type {
	MAT_DIFFUSE = 0,
	MAT_SPECULAR = 1,
	MAT_AMBIANT = 2,
};

class Material
{
public:
	void addColor(unsigned int type,glm::vec3 color);
	bool hasColor();
	
	glm::vec3 diffuse = glm::vec3(0.0f);
	glm::vec3 specular = glm::vec3(0.0f);
	glm::vec3 ambiant = glm::vec3(1.0f);

	float specularExponent = 32.0f;
	float opacity = 1.0f;

private:
	
	bool m_hasColor = false;

};

#endif // MODEL_MATERIAL_HEADER
