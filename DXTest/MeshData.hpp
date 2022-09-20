#pragma once

#include <vector>
#include "Mesh.hpp"

struct MeshData
{
	// Constructors
	MeshData();

	// Attributes
	MeshAttributes attributes;

	std::vector<float> positions;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<UINT> indices;

	// Methods
	float* flattenVertices() const;

	// Builder methods
	static const MeshData buildCubic(const float width, const float height, const float depth, MeshAttributes attributes);
};