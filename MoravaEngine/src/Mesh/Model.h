#pragma once

#include "Mesh/Mesh.h"
#include "Texture/Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>


class Model
{
public:
	Model();
	Model(const std::string& fileName, const std::string& texturesPath = "Textures");
	~Model();

	void LoadModel(const std::string& fileName, const std::string& texturesPath = "Textures");
	void Update(glm::vec3 scale);
	void Render(uint32_t txSlotDiffuse, uint32_t txSlotNormal, bool useNormalMaps);
	void RenderPBR();

	// Getters
	inline const std::vector <Mesh*> GetMeshList() const { return m_MeshList; };

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	void Clear();

private:
	std::vector <Mesh*> m_MeshList;
	std::vector <Texture*> m_TextureList;
	std::vector <Texture*> m_NormalMapList;
	std::vector <unsigned int> m_MeshToTexture;

	std::string m_TexturesPath;
	glm::vec3 m_Scale;

};