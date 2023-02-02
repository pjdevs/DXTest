#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <wrl.h>
#include "Graphics.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class RenderWindow
{
private:
	const Graphics& _gfx;
	HWND _handle;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBufferRTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _backBufferDSView;
	D3D11_TEXTURE2D_DESC _backBufferDesc;

private:
	void createWindow(int width, int height);
	void createSwapChain();
	void createBackBuffer();
	void initImGui();

private:
	static LRESULT CALLBACK WinProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	RenderWindow(const Graphics& gfx, int width, int height);
	~RenderWindow();

	int getWidth() const;
	int getHeight() const;
	const std::unique_ptr<Graphics>& getGfx();

	void beginImGui() const;
	void endImGui() const;
	void present() const;
	
	IDXGISwapChain* getSwapChain() const;
	ID3D11RenderTargetView* getBackBufferRT() const;
	ID3D11DepthStencilView* getBackBufferDS() const;
};

