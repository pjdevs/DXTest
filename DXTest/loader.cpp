#include "loader.hpp"

MeshData* loadMeshData(const std::string& path)
{
	Assimp::Importer* importer = new Assimp::Importer();

	const aiScene* scene = importer->ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcess_GlobalScale |
		aiProcess_MakeLeftHanded);

	if (scene == nullptr)
		throw std::exception(importer->GetErrorString());

	if (!scene->HasMeshes())
		throw std::exception("File has no meshes");

	aiMesh* mesh = scene->mMeshes[0];
	MeshData* data = new MeshData();

	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		data->positions.push_back(mesh->mVertices[i].x);
		data->positions.push_back(mesh->mVertices[i].y);
		data->positions.push_back(mesh->mVertices[i].z);

		data->normals.push_back(mesh->mNormals[i].x);
		data->normals.push_back(mesh->mNormals[i].y);
		data->normals.push_back(mesh->mNormals[i].z);

		data->texCoords.push_back(mesh->mTextureCoords[0][i].x);
		data->texCoords.push_back(mesh->mTextureCoords[0][i].y);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			data->indices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	return data;
}

Texture* loadTexture(const GraphicsDevice& device, const std::string& path)
{
	int imWidth, imHeight, imChannels;
	stbi_uc* imData = stbi_load(path.c_str(), &imWidth, &imHeight, &imChannels, 0);

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	Texture* texture = new Texture(device.getDevice(), imWidth, imHeight, imData, 4 * sizeof(float), DXGI_FORMAT_R8G8B8A8_UINT);

	stbi_image_free(imData);

	return texture;

}

Texture* loadTextureHDR(const GraphicsDevice& device, const std::string& path)
{
	int imWidth, imHeight, imChannels;
	float* imData = stbi_loadf(path.c_str(), &imWidth, &imHeight, &imChannels, 0);

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	Texture* texture = new Texture(device.getDevice(), imWidth, imHeight, imData, 3 * sizeof(float), DXGI_FORMAT_R32G32B32_FLOAT);

	stbi_image_free(imData);

	return texture;
}