#include "Graphics.hpp"

Graphics::Graphics()
	: _device(nullptr), _deviceContext(nullptr)
{
	// Select an adapter
	IDXGIAdapter* pAdapter;
	std::vector<IDXGIAdapter*> vAdapters;
	IDXGIFactory* pFactory = nullptr;

	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
		throw "Failed";

	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
	
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		OutputDebugString(desc.Description);
		OutputDebugString(L"\n");
	}

	pFactory->Release();

	// Create device
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL level;

	HRESULT hr = D3D11CreateDevice(
		vAdapters[1],
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&_device,
		&level,
		&_deviceContext
	);

	if (level != D3D_FEATURE_LEVEL_11_0 || FAILED(hr))
		throw std::exception("Direct3D 11.0 must be supported");
}

Graphics::~Graphics()
{
}

Microsoft::WRL::ComPtr<ID3D11Device> Graphics::getDevice() const
{
	return _device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> Graphics::getDeviceContext() const
{
	return _deviceContext;
}

ID3D11Buffer* Graphics::createBuffer(void* data, size_t sizeInBytes, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags, D3D11_CPU_ACCESS_FLAG cpuAccessFlags) const
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