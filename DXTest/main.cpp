#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <vector>
#include <imgui.h>
#include "GraphicsDevice.hpp"
#include "RenderWindow.hpp"
#include "Shader.hpp"

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
	HRESULT hr;
	
	#pragma region DX
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

	hr = device.getDevice()->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);

	// Check for error
	if (FAILED(hr)) throw new std::exception("Error while creating vertex buffer");

	ID3D11Buffer* vertexBuffers[] = { vertexBuffer, vertexBuffer };
	UINT strides[] = { sizeof(Vertex), sizeof(Vertex) };
	UINT offsets[] = { 0, sizeof(XMFLOAT3) };

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
	hr = device.getDevice()->CreateBuffer(&cbDesc, &cbData, &vsCb);

	// Check for error
	if (FAILED(hr)) throw new std::exception("Error while creating constant buffer");
	#pragma endregion

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

		// Render scene
		viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();
		device.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), blue);
		device.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		device.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.getDeviceContext()->IASetInputLayout(shader.getInputLayout());
		device.getDeviceContext()->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);

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