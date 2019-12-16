#include "Model.h"

void Model::Draw(Shader shader)
{
	for (auto& mesh : meshes)
	{
		mesh.Draw(shader);
	}
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error::Assimp::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (auto i=0;i<node->mNumChildren;++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (auto i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.Position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		vertex.Normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);
		// Assimp allows a model to have up to 8 texture coordinates per vertex, we only use the first
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
	}

	// indices
	for (auto i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[i]);
	}

	// material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMeterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		std::vector<Texture> specularMaps = loadMeterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		// 3. normal maps
		std::vector<Texture> normalMaps = loadMeterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps				  
		std::vector<Texture> heightMaps = loadMeterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMeterialTextures(aiMaterial* mat, aiTextureType texType, std::string typeName)
{
	std::vector<Texture> textures;
	for (auto i = 0; i < mat->GetTextureCount(texType); ++i)
	{
		aiString texPath;
		mat->GetTexture(texType, i, &texPath);
		bool skip = false;
		for (auto j = 0; j < textures_loaded.size(); ++j)
		{
			if (std::strcmp(textures_loaded[j].path.data(), texPath.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{

			Texture texture;
			texture.type = typeName;
			texture.path = texPath.C_Str();
			texture.id = loadTexture(texture.path.c_str());
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}