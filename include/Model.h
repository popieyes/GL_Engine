#pragma once
#include "Common.h"
#include "Shader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include "material/Material.h"


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);


class Model
{
public:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;
	glm::mat4 ModelMat = glm::mat4(1.0f);
	Material& material;
	Model(string const &path, Material& mat) : material(mat)
		{ 
			loadModel(path);
			
		}
	void Draw();
private:

	void loadModel(string const& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};

