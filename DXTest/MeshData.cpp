#include "MeshData.hpp"

MeshData::MeshData()
	: attributes(ALL)
{
}

float* MeshData::flattenVertices() const
{
	size_t size = positions.size();
	int attributeCount = 3; // position
	
	if (texCoords.size() > 0)
	{
		size += texCoords.size();
		attributeCount += 2;
	}

	if (normals.size() > 0)
	{
		size += normals.size();
		attributeCount += 3;
	}

	size_t vertexCount = positions.size() / 3;
	float* vertices = new float[size];

	for (int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
	{
		int firstFloatIndex = 3 * vertexIndex;
		int firstFloatIndexTex = 2 * vertexIndex;
		int flatFirstFloatIndex = attributeCount * vertexIndex;

		vertices[flatFirstFloatIndex] = positions[firstFloatIndex];
		vertices[flatFirstFloatIndex + 1] = positions[firstFloatIndex + 1];
		vertices[flatFirstFloatIndex + 2] = positions[firstFloatIndex + 2];

		if (texCoords.size() > 0)
		{
			vertices[flatFirstFloatIndex + 3] = texCoords[firstFloatIndexTex];
			vertices[flatFirstFloatIndex + 4] = texCoords[firstFloatIndexTex + 1];
		}

		if (normals.size() > 0)
		{
			vertices[flatFirstFloatIndex + 5] = normals[firstFloatIndex];
			vertices[flatFirstFloatIndex + 6] = normals[firstFloatIndex + 1];
			vertices[flatFirstFloatIndex + 7] = normals[firstFloatIndex + 2];
		}
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

const MeshData MeshData::buildNDCRect()
{
	MeshData data;
	data.attributes = (MeshAttributes)(POSITION | TEXCOORD);

	// Front face
	data.positions.push_back(-1.0f);
	data.positions.push_back(-1.0f);
	data.positions.push_back(0.0f);

	data.texCoords.push_back(0.0f);
	data.texCoords.push_back(0.0f);

	data.positions.push_back(-1.0f);
	data.positions.push_back(1.0f);
	data.positions.push_back(0.0f);

	data.texCoords.push_back(0.0f);
	data.texCoords.push_back(1.0f);

	data.positions.push_back(1.0f);
	data.positions.push_back(1.0f);
	data.positions.push_back(0.0f);

	data.texCoords.push_back(1.0f);
	data.texCoords.push_back(1.0f);

	data.positions.push_back(1.0f);
	data.positions.push_back(-1.0f);
	data.positions.push_back(0.0f);

	data.texCoords.push_back(1.0f);
	data.texCoords.push_back(0.0f);

	data.indices.push_back(0);
	data.indices.push_back(1);
	data.indices.push_back(2);
	data.indices.push_back(0);
	data.indices.push_back(2);
	data.indices.push_back(3);

	return data;
}