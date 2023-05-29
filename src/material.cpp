#include "headers/material.h"

void Material::addColor(unsigned int type,glm::vec3 color) {
	
	switch (type) {
	
	case MAT_DIFFUSE:
		this->diffuse = color;
		this->m_hasColor = true;
		break;
	case MAT_AMBIANT:
		if (color != glm::vec3(0.0f))
			this->ambiant = color;
		break;
	case MAT_SPECULAR:
		this->specular = color;
		break;
	}
	
}

bool Material::hasColor() {
	return this->m_hasColor;
}
