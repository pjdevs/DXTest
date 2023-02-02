#include "loader.hpp"

MeshData* loadMeshData(const std::string& path, bool flip)
{
	Assimp::Importer* importer = new Assimp::Importer();

	const aiScene* scene = importer->ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_OptimizeGraph |
		(flip ? aiProcess_FlipWindingOrder : 0) |
		aiProcess_OptimizeMeshes |
		aiProcess_GlobalScale |
		aiProcess_MakeLeftHanded);

	if (scene == nullptr)
	{
		OutputDebugStringA(importer->GetErrorString());
		throw std::exception(importer->GetErrorString());
	}

	if (!scene->HasMeshes())
		throw std::exception("File has no meshes");

	aiMesh* mesh = scene->mMeshes[0];
	MeshData* data = new MeshData();

	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
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

	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		for (unsigned j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			data->indices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	return data;
}

Texture* loadTexture(const Graphics& device, const std::string& path, bool alpha)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* imData = stbi_load(path.c_str(), &width, &height, &channels, alpha ? 4 : 3);

	if ((channels == 4 && !alpha) || (channels == 3 && alpha))
		throw std::exception("Wrong number of channels");

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	int size = width * height;
	stbi_uc* imDataA = new stbi_uc[size * 4];

	for (int i = 0; i < size; ++i)
	{
		int rgba = i * 4;
		int rgb = i * channels;

		imDataA[rgba] = imData[rgb];
		imDataA[rgba + 1] = imData[rgb + 1];
		imDataA[rgba + 2] = imData[rgb + 2];
		imDataA[rgba + 3] = 1;
	}

	Texture* texture = new Texture(
		device,
		width,
		height,
		4 * sizeof(stbi_uc),
		1,
		1,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D_SRV_DIMENSION_TEXTURE2D,
		imDataA
	);

	delete[] imDataA;
	stbi_image_free(imData);

	return texture;

}

Texture* loadTextureHDR(const Graphics& device, const std::string& path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(false);
	float* imData = stbi_loadf(path.c_str(), &width, &height, &channels, 3);

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	int size = width * height;
	float* imDataA = new float[size * 4];

	for (int i = 0; i < size; ++i)
	{
		int rgba = i * 4;
		int rgb = i * 3;

		imDataA[rgba] = imData[rgb];
		imDataA[rgba + 1] = imData[rgb + 1];
		imDataA[rgba + 2] = imData[rgb + 2];
		imDataA[rgba + 3] = 1.0f;
	}

	Texture* texture = new Texture(
		device,
		width,
		height,
		4 * sizeof(float),
		1,
		1,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_USAGE_IMMUTABLE,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D_SRV_DIMENSION_TEXTURE2D,
		imDataA
	);

	stbi_image_free(imData);
	delete[] imDataA;

	return texture;
}