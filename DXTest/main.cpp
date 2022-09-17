#define STB_IMAGE_IMPLEMENTATION 

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
#include "Camera.hpp"
#include "stb_image.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 uv;
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
	SphericalCamera camera(0.f, 0.f, 0.f);
	
	Vertex vertices[] =
	{
		// Triangle 1
		XMFLOAT3(-5.0f, 0.0f, -5.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f, -5.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(-5.0f, 0.0f,  5.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		// Triangle 2
		XMFLOAT3(-5.0f, 0.0f,  5.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f, -5.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3( 5.0f, 0.0f,  5.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
	};
	Mesh mesh(device, vertices, sizeof(Vertex), 6, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	VSConstantBuffer cb {};
	ID3D11Buffer* vsCb = device.createConstantBuffer(&cb, sizeof(VSConstantBuffer));

	char buf[100] = "Hello";
	float clearColor[] = { 0.2f, 0.4f, 0.9f, 1.0f };
	float mouseScrollY = 0.0f;
	CD3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE cbRessource;
	ImGuiIO& io = ImGui::GetIO();

	int imWidth, imHeight, imChannels;
	float* imData = stbi_loadf("Resources/park_parking_4k.hdr", &imWidth, &imHeight, &imChannels, 0);

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = imWidth;
	textureDesc.Height = imHeight;
	textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	D3D11_SUBRESOURCE_DATA textureData = { 0 };
	textureData.pSysMem = imData;
	textureData.SysMemPitch = 3 * sizeof(float) * imWidth;

	ID3D11Texture2D* texture = nullptr;
	HRESULT hr = device.getDevice()->CreateTexture2D(&textureDesc, &textureData, &texture);

	if (FAILED(hr))
		throw std::exception("Cannot create texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* textureView = nullptr;
	hr = device.getDevice()->CreateShaderResourceView(texture, &textureViewDesc, &textureView);

	if (FAILED(hr))
		throw std::exception("Cannot create resource view");

	stbi_image_free(imData);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampler = nullptr;
	hr = device.getDevice()->CreateSamplerState(&samplerDesc, &sampler);

	if (FAILED(hr))
		throw std::exception("Cannot create sampler state");

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
		camera.setSize(window.getWidth(), window.getHeight());
		camera.update(io.MouseDelta.x, io.MouseDelta.y, mouseScrollY, io.MouseDown[1], io.DeltaTime);
		camera.computeMatrices();

		XMStoreFloat4x4(&cb.model, XMMatrixIdentity());
		cb.view = camera.getView();
		cb.projection = camera.getProjection();
		cb.viewPos = camera.getPosition();

		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		// Render scene
		viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();
		device.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), clearColor);
		device.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		device.getDeviceContext()->IASetPrimitiveTopology(mesh.getTopology());
		device.getDeviceContext()->IASetInputLayout(shader.getInputLayout());
		device.getDeviceContext()->IASetVertexBuffers(0, mesh.getNumBuffers(), mesh.getVertexBuffers(), mesh.getVertexStrides(), mesh.getVertexOffsets());

		device.getDeviceContext()->VSSetShader(shader.getVertexShader(), nullptr, 0);
		device.getDeviceContext()->PSSetShader(shader.getPixelShader(), nullptr, 0);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetShaderResources(0, 1, &textureView);
		device.getDeviceContext()->PSSetSamplers(0, 1, &sampler);

		device.getDeviceContext()->Draw(6, 0);

		// Render GUI
		window.beginImGui();

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Benchmark"))
		{
			ImGui::Text("FPS: %f", io.Framerate);
		}

		if (ImGui::CollapsingHeader("View"))
		{
			ImGui::Text("Camera: (%f, %f, %f)", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
			ImGui::ColorEdit3("Clear color", clearColor);
		}
	
		if (ImGui::CollapsingHeader("ImGuiIO"))
		{
			ImGui::Text("Mouse dx: %f", io.MouseDelta.x);
			ImGui::Text("Mouse dy: %f", io.MouseDelta.y);
			ImGui::Text("Mouse wheel: %f", io.MouseWheel);
			ImGui::Text("Mouse right clicked: %d", io.MouseDown[1]);
			ImGui::Text("dt: %f", io.DeltaTime);
		}

		ImGui::End();

		mouseScrollY = io.MouseWheel;

		window.endImGui();
		window.present();
	}

	return 0;
}