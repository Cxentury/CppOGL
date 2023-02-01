#ifndef OBJECT_MODEL_H
#define OBJECT_MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>          
#include <assimp/postprocess.h>

#include "mesh.h"
#include "vector"
#include "iostream"

class Model
{
public:
	Model();
	Model(const std::string& path);
	void loadModel(const std::string& path);
	void parseNodes(aiNode* node, const aiScene* scene, Model& parent, aiMatrix4x4& transform);
	void draw(Shader& shader);
	void addMesh(Mesh mesh);
	Mesh createMesh(aiMesh* mesh, const aiScene* scene, aiMatrix4x4& localTransform);
	Model& addChild(Model& child);

private:
	std::string dir;
	std::vector<Mesh> m_meshes;
	std::vector<Model> m_children;
	std::vector<Texture> loadMaterial(aiMaterial* material, aiTextureType type);
};

#endif // !OBJECT_MODEL_H
