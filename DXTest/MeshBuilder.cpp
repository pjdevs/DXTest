#include "MeshBuilder.hpp"

MeshData::MeshData()
	: attributes(ALL)
{
}

float* MeshData::flattenVertices() const
{
	size_t size = positions.size() + normals.size() + texCoords.size();
	size_t vertexCount = positions.size() / 3;
	float* vertices = new float[size];

	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		int firstFloatIndex = 3 * vertexIndex;
		int firstFloatIndexTex = 2 * vertexIndex;
		int flatFirstFloatIndex = 8 * vertexIndex;

		vertices[flatFirstFloatIndex] = positions[firstFloatIndex];
		vertices[flatFirstFloatIndex + 1] = positions[firstFloatIndex + 1];
		vertices[flatFirstFloatIndex + 2] = positions[firstFloatIndex + 2];

		vertices[flatFirstFloatIndex + 3] = texCoords[firstFloatIndexTex];
		vertices[flatFirstFloatIndex + 4] = texCoords[firstFloatIndexTex + 1];

		vertices[flatFirstFloatIndex + 5] = normals[firstFloatIndex];
		vertices[flatFirstFloatIndex + 6] = normals[firstFloatIndex + 1];
		vertices[flatFirstFloatIndex + 7] = normals[firstFloatIndex + 2];
	}

	return vertices;
}

const MeshData MeshData::buildCubic(const float width, const float height, const float depth, MeshAttributes attributes)
{
	MeshData data;
	data.attributes = attributes;

	const float halfWidth = width / 2.0f;
	const float halfHeight = height / 2.0f;
	const float halfDepth = depth / 2.0f;

	// Front face
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);

	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);

	data.indices.push_back(0);
	data.indices.push_back(1);
	data.indices.push_back(3);
	data.indices.push_back(2);
	data.indices.push_back(3);
	data.indices.push_back(1);

	// Back face
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);

	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);

	data.indices.push_back(4);
	data.indices.push_back(5);
	data.indices.push_back(7);
	data.indices.push_back(6);
	data.indices.push_back(7);
	data.indices.push_back(5);

	// Left face
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);

	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);

	data.indices.push_back(8);
	data.indices.push_back(9);
	data.indices.push_back(11);
	data.indices.push_back(10);
	data.indices.push_back(11);
	data.indices.push_back(9);

	// Right face
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);

	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);

	data.indices.push_back(12);
	data.indices.push_back(13);
	data.indices.push_back(15);
	data.indices.push_back(14);
	data.indices.push_back(15);
	data.indices.push_back(13);

	// Top face
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(halfHeight);
	data.positions.push_back(halfDepth);

	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(1.0f);
	data.normals.push_back(0.0f);

	data.indices.push_back(16);
	data.indices.push_back(17);
	data.indices.push_back(19);
	data.indices.push_back(18);
	data.indices.push_back(19);
	data.indices.push_back(17);

	// Bottom face
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);
	data.positions.push_back(-halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(-halfDepth);
	data.positions.push_back(halfWidth);
	data.positions.push_back(-halfHeight);
	data.positions.push_back(halfDepth);

	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(0.0f);
	data.normals.push_back(-1.0f);
	data.normals.push_back(0.0f);

	data.indices.push_back(20);
	data.indices.push_back(21);
	data.indices.push_back(23);
	data.indices.push_back(22);
	data.indices.push_back(23);
	data.indices.push_back(21);

	return data;
}

MeshData* MeshData::fromFile(const std::string& path)
{
	Assimp::Importer* importer = new Assimp::Importer();

	const aiScene* scene = importer->ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcess_FlipWindingOrder |
		aiProcess_GlobalScale);

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