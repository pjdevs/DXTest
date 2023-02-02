#include "Mesh.hpp"

Mesh::Mesh(const Graphics& device, void* vertices, UINT vertexSize, UINT vertexCount, UINT* indices, UINT indexCount, D3D11_PRIMITIVE_TOPOLOGY topology)
	: _vertexCount(vertexCount), _numBuffers(3), _indexBuffer(nullptr), _indexCount(indexCount), _topology(topology)
{
	ID3D11Buffer* vertexBuffer = device.createBuffer(vertices, vertexSize * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0);
	_indexBuffer = device.createBuffer(indices, sizeof(UINT) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, (D3D11_CPU_ACCESS_FLAG)0);

	_vertexBuffers[0] = vertexBuffer;
	_vertexBuffers[1] = vertexBuffer;
	_vertexBuffers[2] = vertexBuffer;
	_strides[0] = vertexSize;
	_strides[1] = vertexSize;
	_strides[2] = vertexSize;
	_offsets[0] = 0;
	_offsets[1] = sizeof(DirectX::XMFLOAT3);
	_offsets[2] = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2);
}

Mesh::~Mesh()
{
}

ID3D11Buffer** Mesh::getVertexBuffers()
{
	return _vertexBuffers;
}

ID3D11Buffer* Mesh::getIndexBuffer()
{
	return _indexBuffer;
}

UINT* Mesh::getVertexStrides()
{
	return _strides;
}

UINT* Mesh::getVertexOffsets()
{
	return _offsets;
}

UINT Mesh::getNumBuffers() const
{
	return _numBuffers;
}

UINT Mesh::getIndexCount() const
{
	return _indexCount;
}

void Mesh::bind(const Graphics& device)
{
	device.getDeviceContext()->IASetPrimitiveTopology(_topology);
	device.getDeviceContext()->IASetVertexBuffers(0, getNumBuffers(), getVertexBuffers(), getVertexStrides(), getVertexOffsets());
	device.getDeviceContext()->IASetIndexBuffer(getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

void Mesh::draw(const Graphics& device)
{
	device.getDeviceContext()->DrawIndexed(getIndexCount(), 0, 0);
}
