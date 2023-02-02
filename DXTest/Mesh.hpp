#pragma once

#include <exception>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics.hpp"

constexpr auto MESH_ATTRIBUTES = 4;

enum MeshAttributes
{
	POSITION = 1,
	TEXCOORD = 2,
	NORMAL = 4,
	TANGENT = 8,
	ALL = 15
};

class Mesh
{
private:
	ID3D11Buffer* _vertexBuffers[MESH_ATTRIBUTES];
	ID3D11Buffer* _indexBuffer;
	UINT _offsets[MESH_ATTRIBUTES];
	UINT _strides[MESH_ATTRIBUTES];
	UINT _vertexCount;
	UINT _indexCount;
	UINT _numBuffers;
	D3D11_PRIMITIVE_TOPOLOGY _topology;

public:
	Mesh(const Graphics& device, void* vertices, UINT vertexSize, UINT vertexCount, UINT* indices, UINT indexCount, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~Mesh();

	ID3D11Buffer** getVertexBuffers();
	ID3D11Buffer* getIndexBuffer();
	UINT* getVertexStrides();
	UINT* getVertexOffsets();
	UINT getNumBuffers() const;
	UINT getIndexCount() const;

	void bind(const Graphics& device);
	void draw(const Graphics& device);
};