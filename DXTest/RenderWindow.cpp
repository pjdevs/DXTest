#include "RenderWindow.hpp"

LRESULT CALLBACK RenderWindow::WinProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(handle, msg, wParam, lParam))
		return true;
	
	RenderWindow* window = (RenderWindow*)GetWindowLongPtr(handle, GWLP_USERDATA);

	if (msg == WM_SIZE && wParam != SIZE_MINIMIZED && window && window->getSwapChain())
	{
		window->_backBufferDesc.Width = (UINT)LOWORD(lParam);
		window->_backBufferDesc.Height = (UINT)HIWORD(lParam);
		window->_backBufferRTView->Release();
		window->_backBufferDSView->Release();
		window->_backBufferDSView = nullptr;
		window->_backBufferRTView = nullptr;
		window->getSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		window->createBackBuffer();

		return 0;
	}
	else if (msg == WM_DESTROY || msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

void RenderWindow::createWindow(int width, int height)
{
	// Define window style
	WNDCLASS wc = { 0 };
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WinProc;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"DXTest";
	RegisterClass(&wc);

	// Create the window
	_handle = CreateWindow(L"DXTest", L"DX11 RenderWindow",
		WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
		100, 100, width, height,
		nullptr, nullptr, nullptr, nullptr);

	SetWindowLongPtr(_handle, GWLP_USERDATA, (LONG_PTR)this);
}

void RenderWindow::createSwapChain()
{
	// Get the factory from device
	IDXGIDevice* pDXGIDevice = nullptr;
	HRESULT hr = _gfx.getDevice()->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

	if (FAILED(hr))
		throw new std::exception("Cannot get the DXGIDevice");

	IDXGIAdapter* pDXGIAdapter = nullptr;
	hr = pDXGIDevice->GetAdapter(&pDXGIAdapter);

	if (FAILED(hr))
		throw new std::exception("Cannot get the DXGIAdaptater");

	IDXGIFactory* pIDXGIFactory = nullptr;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);

	if (FAILED(hr))
		throw new std::exception("Cannot get the DXGIFactory");

	// Define and create the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = _handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	hr = pIDXGIFactory->CreateSwapChain(
		_gfx.getDevice().Get(),
		&swapChainDesc,
		&_swapChain
	);

	if (FAILED(hr))
		throw new std::exception("Cannot create the DXGISwapChain");
}

void RenderWindow::createBackBuffer()
{
	// Create render target
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture = nullptr;

	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBufferTexture.GetAddressOf());

	if (FAILED(hr))
		throw new std::exception("Cannot get the backbuffer");

	hr = _gfx.getDevice()->CreateRenderTargetView(backBufferTexture.Get(), nullptr, _backBufferRTView.GetAddressOf());

	if (FAILED(hr))
		throw new std::exception("Error while creating render target view");

	// Get backbuffer desc to create depth stencil
	backBufferTexture->GetDesc(&_backBufferDesc);

	// Create depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = _backBufferDesc.Width;
	depthStencilDesc.Height = _backBufferDesc.Height;
	depthStencilDesc.MipLevels = _backBufferDesc.MipLevels;
	depthStencilDesc.ArraySize = _backBufferDesc.ArraySize;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = _backBufferDesc.SampleDesc.Count;
	depthStencilDesc.SampleDesc.Quality = _backBufferDesc.SampleDesc.Quality;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = _gfx.getDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);

	// Check for error
	if (FAILED(hr))
		throw new std::exception("Error while creating depth stencil");

	hr = _gfx.getDevice()->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, _backBufferDSView.GetAddressOf());

	// Check for error
	if (FAILED(hr))
		throw new std::exception("Error while creating depth stencil view");
}

void RenderWindow::initImGui()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(_handle);
	ImGui_ImplDX11_Init(_gfx.getDevice().Get(), _gfx.getDeviceContext().Get());
}

RenderWindow::RenderWindow(const Graphics& gfx, int width, int height)
	: _gfx(gfx), _handle(0), _swapChain(nullptr), _backBufferRTView(nullptr), _backBufferDSView(nullptr)
{
	createWindow(width, height);
	createSwapChain();
	createBackBuffer();
	initImGui();
}

RenderWindow::~RenderWindow()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

int RenderWindow::getWidth() const
{
	return _backBufferDesc.Width;
}

int RenderWindow::getHeight() const
{
	return _backBufferDesc.Height;
}

void RenderWindow::beginImGui() const
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void RenderWindow::endImGui() const
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void RenderWindow::present() const
{
	_swapChain->Present(1, 0);
}

IDXGISwapChain* RenderWindow::getSwapChain() const
{
	return _swapChain.Get();
}

ID3D11RenderTargetView* RenderWindow::getBackBufferRT() const
{
	return _backBufferRTView.Get();
}

ID3D11DepthStencilView* RenderWindow::getBackBufferDS() const
{
	return _backBufferDSView.Get();
}
