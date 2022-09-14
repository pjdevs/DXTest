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
