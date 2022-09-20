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
		throw std::exception("Direct3D 11.1 must be supported");
}

ID3D11Device* GraphicsDevice::getDevice() const
{
	return _device;
}

ID3D11DeviceContext* GraphicsDevice::getDeviceContext() const
{
	return _deviceContext;
}

ID3D11Buffer* GraphicsDevice::createBuffer(void* data, size_t sizeInBytes, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccessFlags) const
{
	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeInBytes;
	cbDesc.Usage = usage;
	cbDesc.BindFlags = bindFlags;
	cbDesc.CPUAccessFlags = cpuAccessFlags;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbData = { 0 };
	cbData.pSysMem = data;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	// Create the buffer.
	ID3D11Buffer* buffer = nullptr;
	HRESULT hr = _device->CreateBuffer(&cbDesc, &cbData, &buffer);

	if (FAILED(hr))
		throw std::exception("Error while creating buffer");

	return buffer;
}