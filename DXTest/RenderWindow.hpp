#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "GraphicsDevice.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class RenderWindow
{
private:
	HWND _handle;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _backBufferRTView;
	ID3D11DepthStencilView* _backBufferDSView;
	D3D11_TEXTURE2D_DESC _backBufferDesc;

	const GraphicsDevice& _device;

private:
	void createWindow(int width, int height);
	void createSwapChain(HWND handle, ID3D11Device* device);
	void createBackBuffer(ID3D11Device* device, IDXGISwapChain* swapChain);
	void initImGui(HWND handle, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

private:
	static LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	RenderWindow(const GraphicsDevice& device, int width, int height);
	~RenderWindow();

	int getWidth() const;
	int getHeight() const;

	void beginImGui() const;
	void endImGui() const;
	void present() const;
	
	IDXGISwapChain* getSwapChain() const;
	ID3D11RenderTargetView* getBackBufferRT() const;
	ID3D11DepthStencilView* getBackBufferDS() const;
};

