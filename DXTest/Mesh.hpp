#pragma once

#include <exception>
#include <d3d11.h>
#include <DirectXMath.h>
#include "GraphicsDevice.hpp"

constexpr auto MESH_ATTRIBUTES = 3;

class Mesh
{
private:
	ID3D11Buffer* _vertexBuffers[MESH_ATTRIBUTES];
	UINT _offsets[MESH_ATTRIBUTES];
	UINT _strides[MESH_ATTRIBUTES];
	D3D11_PRIMITIVE_TOPOLOGY _topology;
	size_t _vertexCount;

public:
	Mesh(const GraphicsDevice& device, void* vertices, UINT vertexSize, UINT vertexCount, D3D11_PRIMITIVE_TOPOLOGY topology);
	~Mesh();

	ID3D11Buffer** getVertexBuffers();
	UINT* getVertexStrides();
	UINT* getVertexOffsets();
	D3D11_PRIMITIVE_TOPOLOGY getTopology() const;
	UINT getNumBuffers() const;
};