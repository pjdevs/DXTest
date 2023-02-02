#pragma once

#include <memory>
#include <exception>
#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include <wrl.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

class Graphics
{
	friend class GraphicsResource;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext;

public:
	Graphics();
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();

	Microsoft::WRL::ComPtr<ID3D11Device> getDevice() const;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> getDeviceContext() const;

	ID3D11Buffer* createBuffer(void* data, size_t sizeInBytes, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccessFlags) const;
};

