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

static const XMMATRIX rotations[6] = {
	XMMatrixRotationY(-XM_PIDIV2), // Look: +x Index: 0
	XMMatrixRotationY(XM_PIDIV2), // Look: -x Index: 1
	XMMatrixRotationX(XM_PIDIV2), // Look: +y Index: 2
	XMMatrixRotationX(-XM_PIDIV2), // Look: -y Index: 3
	XMMatrixIdentity(),            // Look: +z Index: 4
	XMMatrixRotationY(XM_PI),      // Look: -z Index: 5
};

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
	Shader shader(device, L"Phong.hlsl", normalDesc, ARRAYSIZE(normalDesc));
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

	Mesh* cubeMesh = loadMesh(device, "Resources/cube.obj", true);
	Texture* texture = loadTextureHDR(device, "Resources/park_parking_4k.hdr");
	TextureCube tc(device, 1024, 1024, 1);
	Shader projectShader(device, L"project.hlsl", positionDesc, ARRAYSIZE(positionDesc));

	ID3D11DepthStencilView* dsView = nullptr;
	ID3D11Texture2D* depthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
	depthStencilDesc.Width = 1024;
	depthStencilDesc.Height = 1024;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = device.getDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilTexture);

	// Check for error
	if (FAILED(hr))
		throw new std::exception("Error while creating depth stencil");

	hr = device.getDevice()->CreateDepthStencilView(depthStencilTexture, nullptr, &dsView);

	// Check for error
	if (FAILED(hr))
		throw new std::exception("Error while creating depth stencil view");

	// Fill cubemap
	XMMATRIX viewMatrices[6] =
	{
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f))),
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f))),
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f,1.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
	};

	for (int i = 0; i < 6; ++i)
	{
		viewport = CD3D11_VIEWPORT(0.f, 0.f, 1024.f, 1024.f);
		ID3D11RenderTargetView* views = tc.getCubeFaceRTView(i);
		device.getDeviceContext()->OMSetRenderTargets(1, &views, dsView);
		device.getDeviceContext()->RSSetViewports(1, &viewport);
		device.getDeviceContext()->ClearRenderTargetView(views, clearColor);
		device.getDeviceContext()->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		device.getDeviceContext()->OMSetDepthStencilState(defaultDs, 0);

		XMStoreFloat4x4(&cb.model, XMMatrixIdentity());
		XMStoreFloat4x4(&cb.view, viewMatrices[i]);
		XMStoreFloat4x4(
			&cb.projection,
			XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(90.f), 1.0f, 0.1f, 10.f))
		);
		device.getDeviceContext()->Map(vsCb, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbRessource);
		memcpy(cbRessource.pData, &cb, sizeof(VSConstantBuffer));
		device.getDeviceContext()->Unmap(vsCb, 0);

		projectShader.bind(device);
		device.getDeviceContext()->VSSetConstantBuffers(0, 1, &vsCb);
		texture->bind(device, 0);
		cubeMesh->bind(device);
		cubeMesh->draw(device);
	}

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
	UINT triedreIndices[] =
	{
		0, 1, 2, 3, 4, 5
	};
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
			XMQuaternionIdentity(),
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
		texture->bind(device, 0);
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

		ImGui::DragFloat3("Position", (float*)(&position));
		ImGui::DragFloat3("Rotation", (float*)(&rotation));
		ImGui::DragFloat3("Scale", (float*)(&scale));
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