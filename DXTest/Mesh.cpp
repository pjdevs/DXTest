#include "Mesh.hpp"

Mesh::Mesh(const GraphicsDevice& device, void* vertices, UINT vertexSize, UINT vertexCount, D3D11_PRIMITIVE_TOPOLOGY topology)
	: _topology(topology), _vertexCount(vertexCount)
{
	ID3D11Buffer* vertexBuffer = device.createVertexBuffer(vertices, vertexSize * vertexCount);

	_vertexBuffers[0] = vertexBuffer;
	_vertexBuffers[1] = vertexBuffer;
	_strides[0] = vertexSize;
	_strides[1] = vertexSize;
	_offsets[0] = 0;
	_offsets[1] = sizeof(DirectX::XMFLOAT3);
}

Mesh::~Mesh()
{
}

ID3D11Buffer** Mesh::getVertexBuffers()
{
	return _vertexBuffers;
}

UINT* Mesh::getVertexStrides()
{
	return _strides;
}

UINT* Mesh::getVertexOffsets()
{
	return _offsets;
}

D3D11_PRIMITIVE_TOPOLOGY Mesh::getTopology()
{
	return _topology;
}