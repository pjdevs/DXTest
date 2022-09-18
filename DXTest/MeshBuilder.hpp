#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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
	// static const MeshData buildCube(const float size);
	static MeshData* fromFile(const std::string& path);
};