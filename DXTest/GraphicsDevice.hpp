#pragma once

#include <exception>
#include <d3d11.h>
#include <dxgi.h>
#include <vector>

class GraphicsDevice
{
private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

public:
	GraphicsDevice();

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;

	ID3D11Buffer* createBuffer(void* data, size_t sizeInBytes, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccessFlags) const;
};

