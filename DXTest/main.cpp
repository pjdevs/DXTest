#define STB_IMAGE_IMPLEMENTATION 
#define NOMINMAX

#include <imgui.h>
#include "GraphicsDevice.hpp"
#include "RenderWindow.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "MeshData.hpp"
#include "loader.hpp"

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

Mesh* loadMesh(const GraphicsDevice& device, const std::string& path, bool flip = false)
{
	MeshData* data = loadMeshData(path, flip);
	float* vertices = data->flattenVertices();
	Mesh* mesh = new Mesh(device, vertices, sizeof(float) * 8, data->positions.size() / 3, data->indices.data(), data->indices.size());
	delete vertices;
	delete data;

	return mesh;
}

int CALLBACK WinMain(_In_ HINSTANCE appInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdCount) {

	// Shader layouts
	D3D11_INPUT_ELEMENT_DESC normalDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC positionDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC colorDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Init device, window, swapchain
	GraphicsDevice device;
	RenderWindow window(device, 800, 600);
	Shader shader(device, L"IBL.hlsl", normalDesc, ARRAYSIZE(normalDesc));
	Shader cubemapShader(device, L"cubemap.hlsl", positionDesc, ARRAYSIZE(positionDesc));
	SphericalCamera camera(0.f, 0.f, 0.f);

	VSConstantBuffer cb {};
	ID3D11Buffer* vsCb = device.createBuffer(&cb, sizeof(VSConstantBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);

	Asset assets[] =
	{
		{ "Plane", "Resources/plane.obj", false },
		{ "Cube", "Resources/cube.obj", false },
		{ "Sphere", "Resources/sphere.obj", false },
		{ "Smooth sphere", "Resources/sphere_smooth.obj", false },
		{ "Characters", "Resources/Characters.fbx", false },
		{ "Damaged Helmet", "Resources/DamagedHelmet/DamagedHelmet.gltf", false }
	};

	Mesh* mesh = loadMesh(device, assets[0].path);

	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };
	dsDesc.BackFace = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };

	ID3D11DepthStencilState* cubeDs = nullptr;
	HRESULT hr = device.getDevice()->CreateDepthStencilState(&dsDesc, &cubeDs);

	ID3D11DepthStencilState* defaultDs = nullptr;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	hr = device.getDevice()->CreateDepthStencilState(&dsDesc, &defaultDs);

	if (FAILED(hr))
		throw "Bite";

	float clearColor[] = { 0.2f, 0.4f, 0.9f, 1.0f };
	XMFLOAT3 position(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
	float mouseScrollY = 0.0f;
	bool drawTriedre = false;
	CD3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE cbRessource;
	ImGuiIO& io = ImGui::GetIO();
	MSG msg = { 0 };

	#pragma region Project cubemap
	Mesh* cubeMesh = loadMesh(device, "Resources/cube.obj", true);
	Texture* texture = loadTextureHDR(device, "Resources/shanghai_bund_4k.hdr");
	TextureCube tc(device, 256, 256, 1);
	Shader projectShader(device, L"project.hlsl", positionDesc, ARRAYSIZE(positionDesc));
	CubeCamera cubeCamera;

	// Fill cubemap
	viewport = CD3D11_VIEWPORT(0.f, 0.f, 256.f, 256.f);
	device.getDeviceContext()->RSSetViewports(1, &viewport);
	device.getDeviceContext()->OMSetDepthStencilState(defaultDs, 0);
	XMStoreFloat4x4(&cb.model, XMMatrixIdentity());
	cb.projection = cubeCamera.getProjection();

	projectShader.bind(device);
	device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
	texture->bind(device, 0);
	cubeMesh->bind(device);

	for (int i = 0; i < 6; ++i)
	{
		ID3D11RenderTargetView* views = tc.getCubeFaceRTView(i);
		device.getDeviceContext()->ClearRenderTargetView(views, clearColor);
		device.getDeviceContext()->OMSetRenderTargets(1, &views, nullptr);

		cubeCamera.face(i);
		cb.view = cubeCamera.getView();
		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		cubeMesh->draw(device);
	}
	#pragma endregion

	#pragma region Irradiance
	TextureCube irradiance(device, 32, 32, 1);
	Shader irradianceShader(device, L"irradiance.hlsl", positionDesc, ARRAYSIZE(positionDesc));

	viewport = CD3D11_VIEWPORT(0.0f, 0.0f, 32.0f, 32.0f);
	device.getDeviceContext()->RSSetViewports(1, &viewport);

	irradianceShader.bind(device);
	device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
	cubeMesh->bind(device);

	for (int i = 0; i < 6; ++i)
	{
		ID3D11RenderTargetView* views = irradiance.getCubeFaceRTView(i);
		device.getDeviceContext()->ClearRenderTargetView(views, clearColor);
		device.getDeviceContext()->OMSetRenderTargets(1, &views, nullptr);
		tc.bind(device, 0);

		cubeCamera.face(i);
		cb.view = cubeCamera.getView();
		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		cubeMesh->draw(device);
	}
	#pragma endregion

	Shader colorShader(device, L"color.hlsl", colorDesc, ARRAYSIZE(colorDesc));
	float triedreVertices[] =
	{
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 1.0f,
	};
	UINT triedreIndices[] = { 0, 1, 2, 3, 4, 5 };
	Mesh triedre(device, triedreVertices, sizeof(float) * 6, 6, triedreIndices, 6, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// Main loop
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		#pragma region Update
		camera.setSize(window.getWidth(), window.getHeight());
		camera.update(io.MouseDelta.x, io.MouseDelta.y, mouseScrollY, io.MouseDown[1], io.DeltaTime);
		camera.computeMatrices();

		XMStoreFloat4x4(&cb.model, XMMatrixTranspose(XMMatrixAffineTransformation(
			XMLoadFloat3(&scale),
			XMLoadFloat3(&position),
			XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)),
			XMLoadFloat3(&position)
		)));
		cb.view = camera.getView();
		cb.projection = camera.getProjection();
		cb.viewPos = camera.getPosition();
		#pragma endregion

		#pragma region Render scene
		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();
		ID3D11ShaderResourceView* views = texture->getTextureView();
		ID3D11SamplerState* samplers = texture->getSampler();

		device.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), clearColor);
		device.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		device.getDeviceContext()->OMSetDepthStencilState(defaultDs, 0);

		shader.bind(device);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		device.getDeviceContext()->PSSetConstantBuffers(0, 1, &vsCb);
		irradiance.bind(device, 0);
		mesh->bind(device);
		mesh->draw(device);

		if (drawTriedre)
		{
			colorShader.bind(device);
			device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
			triedre.bind(device);
			triedre.draw(device);
		}

		// Cubemap
		device.getDeviceContext()->OMSetDepthStencilState(cubeDs, 0);

		cubemapShader.bind(device);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		tc.bind(device, 0);
		cubeMesh->bind(device);
		cubeMesh->draw(device);
		#pragma endregion

		#pragma region Render GUI
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
				shader.reload(device);
				colorShader.reload(device);
				cubemapShader.reload(device);
			}

			ImGui::Checkbox("Show axes", &drawTriedre);
		}

		ImGui::End();

		ImGui::Begin("Scene");

		if (ImGui::BeginListBox("Assets"))
		{
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
		}

		ImGui::DragFloat3("Position", (float*)(&position), 0.1f);
		ImGui::DragFloat3("Rotation", (float*)(&rotation), 0.1f);
		ImGui::DragFloat3("Scale", (float*)(&scale), 0.1f);
		ImGui::End();
		
		mouseScrollY = io.MouseWheel;

		window.endImGui();
		#pragma endregion

		window.present();
	}

	delete mesh;
	delete cubeMesh;
	delete texture;
	vsCb->Release();
	cubeDs->Release();
	defaultDs->Release();

	return 0;
}