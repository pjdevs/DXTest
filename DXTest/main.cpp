#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>
#include <imgui.h>
#include "GraphicsDevice.hpp"
#include "RenderWindow.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"

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

int CALLBACK WinMain(_In_ HINSTANCE appInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdCount) {

	// Init device, window, swapchain
	GraphicsDevice device;
	RenderWindow window(device, 800, 600);
	Shader shader(device, L"Phong.hlsl");
	
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
	Mesh mesh(device, vertices, sizeof(Vertex), 6, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	VSConstantBuffer cb {};
	const XMFLOAT3 viewPos(3.0f, 3.0f, 3.0f);
	XMStoreFloat4x4(&cb.model, XMMatrixIdentity());
	XMStoreFloat4x4(&cb.view, XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(viewPos.x, viewPos.y, viewPos.z, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f))));
	XMStoreFloat4x4(&cb.projection, XMMatrixTranspose(XMMatrixPerspectiveFovRH(0.7f, 800.f / 600.f, 0.1f, 100.f)));
	cb.viewPos = viewPos;

	ID3D11Buffer* vsCb = device.createConstantBuffer(&cb, sizeof(VSConstantBuffer));

	char buf[100] = "Hello";
	float blue[] = { 0.2f, 0.4f, 0.9f, 1.0f };
	CD3D11_VIEWPORT viewport;

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

		// Update
		int* a[3];
		int** b = a;

		// Render scene
		viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();
		device.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), blue);
		device.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		device.getDeviceContext()->IASetPrimitiveTopology(mesh.getTopology());
		device.getDeviceContext()->IASetInputLayout(shader.getInputLayout());
		device.getDeviceContext()->IASetVertexBuffers(0, 2, mesh.getVertexBuffers(), mesh.getVertexStrides(), mesh.getVertexOffsets());

		device.getDeviceContext()->VSSetShader(shader.getVertexShader(), nullptr, 0);
		device.getDeviceContext()->PSSetShader(shader.getPixelShader(), nullptr, 0);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetConstantBuffers(0, 1, &vsCb);
		
		device.getDeviceContext()->Draw(6, 0);

		// Render GUI
		window.beginImGui();

		ImGui::Begin("Sample");
		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Save"));
		// Do nothing
		ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
		ImGui::SliderFloat("float", &blue[0], 0.0f, 1.0f);
		ImGui::End();

		window.endImGui();
		window.present();
	}

	return 0;
}