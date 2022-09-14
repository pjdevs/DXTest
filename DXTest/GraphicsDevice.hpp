#pragma once

#include <exception>
#include <d3d11.h>

class GraphicsDevice
{
private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

public:
	GraphicsDevice();

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;
};

