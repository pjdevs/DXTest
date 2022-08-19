#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
};

struct VSConstantBuffer
{
	XMFLOAT4X4 model;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT3 viewPos;
	float padding;
};

LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY ||
		msg == WM_CLOSE) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, msg, wparam, lparam);
}

void checkDXError(HRESULT hr, LPCWSTR message)
{
	if (FAILED(hr))
	{
		MessageBox(nullptr, message, L"Error", MB_OK);
		exit(1);
	}
}

HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int CALLBACK WinMain(_In_ HINSTANCE appInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdCount) {

	// Define window style
	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WinProc;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"DXTest";
	RegisterClass(&wc);

	// Create the window
	HWND handle = CreateWindow(L"DXTest", L"DXTest Window",
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		100, 100, WINDOW_WIDTH, WINDOW_HEIGHT,
		nullptr, nullptr, nullptr, nullptr);

	// Create DX11 device stuff
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	HRESULT hr;

	// Define our swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };

	// Create the swap chain, device and device context
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
		featureLevels, 1, D3D11_SDK_VERSION,
		&swapChainDesc, &swapChain,
		&device, nullptr, &deviceContext);

	// Check for error
	checkDXError(hr, L"Error creating DX11");

	// Create render target
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11Texture2D* backBufferTexture = nullptr;
	
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);

	// Check for error
	checkDXError(hr, L"Cannot get the backbuffer");

	hr = device->CreateRenderTargetView(backBufferTexture, nullptr, &renderTargetView);

	// Check for error
	checkDXError(hr, L"Error while creating render target view");

	// Get backbuffer desc to create depth stencil
	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBufferTexture->GetDesc(&backBufferDesc);

	// Create depth stencil
	ID3D11Texture2D* depthStencilTexture = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = backBufferDesc.Width;
	depthStencilDesc.Height = backBufferDesc.Height;
	depthStencilDesc.MipLevels = backBufferDesc.MipLevels;
	depthStencilDesc.ArraySize = backBufferDesc.ArraySize;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = backBufferDesc.SampleDesc.Count;
	depthStencilDesc.SampleDesc.Quality = backBufferDesc.SampleDesc.Quality;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);

	// Check for error
	checkDXError(hr, L"Error while creating depth stencil");

	hr = device->CreateDepthStencilView(depthStencilTexture, nullptr, &depthStencilView);

	// Check for error
	checkDXError(hr, L"Error while creating depth stencil view");

	// Release textures
	backBufferTexture->Release();
	depthStencilTexture->Release();
	
	// Create a vertex buffer
	Vertex vertices[] =
	{
		// Triangle 1
		XMFLOAT3(-5.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(-5.0f, 0.0f,  5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		// Triangle 2
		XMFLOAT3(-5.0f, 0.0f,  5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f,  5.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
	};
	ID3D11Buffer* vertexBuffer = nullptr;
	D3D11_BUFFER_DESC vbDesc = CD3D11_BUFFER_DESC(6 * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vbData = { 0 };
	vbData.pSysMem = vertices;

	hr = device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);

	// Check for error
	checkDXError(hr, L"Error while creating vertex buffer");

	// Creating shaders
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	hr = CompileShader(L"Phong.hlsl", "VSMain", "vs_5_0", &vsBlob);
	checkDXError(hr, L"Error while compiling vertex shader");

	hr = CompileShader(L"Phong.hlsl", "PSMain", "ps_5_0", &psBlob);
	checkDXError(hr, L"Error while compiling pixel shader");

	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
	checkDXError(hr, L"Error while creating vertex shader");
	
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
	checkDXError(hr, L"Error while creating pixel shader");

	// Input layout
	ID3D11InputLayout* inputLayout = nullptr;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = device->CreateInputLayout(inputElementDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);

	ID3D11Buffer* vertexBuffers[] = { vertexBuffer, vertexBuffer };
	UINT strides[] = { sizeof(Vertex), sizeof(Vertex) };
	UINT offsets[] = { 0, sizeof(XMFLOAT3) };

	// Check for error
	checkDXError(hr, L"Error while creating input layer");
	
	// Release shader compiled code
	vsBlob->Release();
	psBlob->Release();

	// Create constant buffer
	ID3D11Buffer* vsCb = nullptr;

	VSConstantBuffer cb {};
	const XMFLOAT3 viewPos(3.0f, 3.0f, 3.0f);
	XMStoreFloat4x4(&cb.model, XMMatrixIdentity());
	XMStoreFloat4x4(&cb.view, XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(viewPos.x, viewPos.y, viewPos.z, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f))));
	XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(XMMatrixPerspectiveFovRH(0.7f, 800.f / 600.f, 0.1f, 100.f)));
	cb.viewPos = viewPos;

	D3D11_BUFFER_DESC cbDesc = { 0 };
	cbDesc.ByteWidth = sizeof(VSConstantBuffer);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA cbData = { 0 };
	cbData.pSysMem = &cb;
	cbData.SysMemPitch = 0;
	cbData.SysMemSlicePitch = 0;

	// Create the buffer.
	hr = device->CreateBuffer(&cbDesc, &cbData, &vsCb);

	// Check for error
	checkDXError(hr, L"Error while creating constant buffer");

	//// Create depth stencil state
	//D3D11_DEPTH_STENCIL_DESC dsDesc {};
	//// Depth test parameters
	//dsDesc.DepthEnable = true;
	//dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//// Stencil test parameters
	//dsDesc.StencilEnable = true;
	//dsDesc.StencilReadMask = 0xFF;
	//dsDesc.StencilWriteMask = 0xFF;
	//// Stencil operations if pixel is front-facing
	//dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//// Stencil operations if pixel is back-facing
	//dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//// Create depth stencil state
	//ID3D11DepthStencilState* dsState;
	//hr = device->CreateDepthStencilState(&dsDesc, &dsState);

	//deviceContext->OMSetDepthStencilState(dsState, 0);

	float blue[] = { 0.2f, 0.4f, 0.9f, 1.0f };
	auto viewport = CD3D11_VIEWPORT(0.f, 0.f, backBufferDesc.Width, backBufferDesc.Height);

	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		deviceContext->RSSetViewports(1, &viewport);
		deviceContext->ClearRenderTargetView(renderTargetView, blue);
		deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetInputLayout(inputLayout);
		deviceContext->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

		deviceContext->VSSetShader(vertexShader, nullptr, 0);
		deviceContext->PSSetShader(pixelShader, nullptr, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &vsCb);
		deviceContext->PSSetConstantBuffers(0, 1, &vsCb);
		
		deviceContext->Draw(6, 0);

		swapChain->Present(1, 0);
	}

	return 0;
}