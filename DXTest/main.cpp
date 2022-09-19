#define STB_IMAGE_IMPLEMENTATION 
#define NOMINMAX

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
#include "Texture.hpp"
#include "MeshBuilder.hpp"
#include <array>

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

struct Asset
{
	std::string name;
	std::string path;
	bool isSelected;
};

Mesh* loadMesh(const GraphicsDevice& device, const std::string& path)
{
	MeshData* data = MeshData::fromFile(path);
	float* vertices = data->flattenVertices();
	Mesh* mesh = new Mesh(device, vertices, sizeof(float) * 8, data->positions.size() / 3, data->indices.data(), data->indices.size());
	delete vertices;
	delete data;

	return mesh;
}

int CALLBACK WinMain(_In_ HINSTANCE appInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdCount) {

	// Init device, window, swapchain
	GraphicsDevice device;
	RenderWindow window(device, 800, 600);
	Shader shader(device.getDevice(), L"Phong.hlsl");
	SphericalCamera camera(0.f, 0.f, 0.f);

	VSConstantBuffer cb {};
	ID3D11Buffer* vsCb = device.createBuffer(&cb, sizeof(VSConstantBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);

	int imWidth, imHeight, imChannels;
	float* imData = stbi_loadf("Resources/park_parking_4k.hdr", &imWidth, &imHeight, &imChannels, 0);

	if (imData == nullptr)
		throw std::exception("Cannot load texture file");

	Texture texture(device.getDevice(), imWidth, imHeight, imData, 3 * sizeof(float), DXGI_FORMAT_R32G32B32A32_FLOAT);

	Asset assets[] =
	{
		{ "Plane", "Resources/plane.obj", false },
		{ "Cube", "Resources/cube.obj", false },
		{ "Sphere", "Resources/sphere.obj", false },
		{ "Characters", "Resources/Characters.fbx", false },
	};

	Mesh* mesh = loadMesh(device, assets[0].path);

	char buf[100] = "Hello";
	float clearColor[] = { 0.2f, 0.4f, 0.9f, 1.0f };
	XMFLOAT3 position(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
	float mouseScrollY = 0.0f;
	CD3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE cbRessource;
	ImGuiIO& io = ImGui::GetIO();

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

		XMStoreFloat4x4(&cb.model, XMMatrixTranspose(XMMatrixAffineTransformation(
			XMLoadFloat3(&scale),
			XMLoadFloat3(&position),
			XMQuaternionIdentity(),
			XMLoadFloat3(&position)
		)));
		cb.view = camera.getView();
		cb.projection = camera.getProjection();
		cb.viewPos = camera.getPosition();

		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		// Render scene
		viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();
		ID3D11ShaderResourceView* views = texture.getTextureView();
		ID3D11SamplerState* samplers = texture.getSampler();
		device.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), clearColor);
		device.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		device.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.getDeviceContext()->IASetInputLayout(shader.getInputLayout());
		device.getDeviceContext()->IASetVertexBuffers(0, mesh->getNumBuffers(), mesh->getVertexBuffers(), mesh->getVertexStrides(), mesh->getVertexOffsets());
		device.getDeviceContext()->IASetIndexBuffer(mesh->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		device.getDeviceContext()->VSSetShader(shader.getVertexShader(), nullptr, 0);
		device.getDeviceContext()->PSSetShader(shader.getPixelShader(), nullptr, 0);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetShaderResources(0, 1, &views);
		device.getDeviceContext()->PSSetSamplers(0, 1, &samplers);

		device.getDeviceContext()->DrawIndexed(mesh->getIndexCount(), 0, 0);

		// Render GUI
		window.beginImGui();

		ImGui::Begin("Debug");

		if (ImGui::CollapsingHeader("Benchmark"))
		{
			ImGui::Text("FPS: %f", io.Framerate);
			ImGui::Text("dt: %f", io.DeltaTime);
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
		}

		if (ImGui::CollapsingHeader("Tools"))
		{
			if (ImGui::Button("Reload shaders"))
			{
				shader.reload(device.getDevice());
			}
		}

		ImGui::End();

		ImGui::Begin("Scene");

		ImGui::BeginListBox("Assets");
		for (int i = 0; i < IM_ARRAYSIZE(assets); ++i)
		{
			bool wasSelected = assets[i].isSelected;
			assets[i].isSelected = false;

			if (ImGui::Selectable(assets[i].name.c_str(), &assets[i].isSelected, ImGuiSelectableFlags_DontClosePopups))
			{
				if (!wasSelected)
				{
					delete mesh;
					mesh = loadMesh(device, assets[i].path);
				}
			}
		}
		ImGui::EndListBox();

		ImGui::DragFloat3("Position", (float*)(&position));
		ImGui::DragFloat3("Rotation", (float*)(&rotation));
		ImGui::DragFloat3("Scale", (float*)(&scale));
		ImGui::End();
		
		mouseScrollY = io.MouseWheel;

		window.endImGui();
		window.present();
	}

	return 0;
}