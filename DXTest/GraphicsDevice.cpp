#include "GraphicsDevice.hpp"

GraphicsDevice::GraphicsDevice()
	: _device(nullptr), _deviceContext(nullptr)
{
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL level;

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&_device,
		&level,
		&_deviceContext
	);

	if (level != D3D_FEATURE_LEVEL_11_1 || FAILED(hr))
		throw new std::exception("Direct3D 11.1 must be supported");
}

ID3D11Device* GraphicsDevice::getDevice() const
{
	return _device;
}

ID3D11DeviceContext* GraphicsDevice::getDeviceContext() const
{
	return _deviceContext;
}

ID3D11Buffer* GraphicsDevice::createVertexBuffer(void* vertices, size_t sizeInBytes) const
{
	ID3D11Buffer* vertexBuffer = nullptr;
	D3D11_BUFFER_DESC vbDesc = CD3D11_BUFFER_DESC(sizeInBytes, D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = vertices;

	HRESULT hr = _device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);

	if (FAILED(hr))
		throw new std::exception("Error while creating vertex buffer");

	return vertexBuffer;
}

ID3D11Buffer* GraphicsDevice::createConstantBuffer(void* data, size_t sizeInBytes) const
{
	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeInBytes;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbData = { 0 };
	cbData.pSysMem = data;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	// Create the buffer.
	ID3D11Buffer* constantBuffer = nullptr;
	HRESULT hr = _device->CreateBuffer(&cbDesc, &cbData, &constantBuffer);

	if (FAILED(hr))
		throw new std::exception("Error while creating constant buffer");

	return constantBuffer;
}
