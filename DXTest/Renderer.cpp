#include "Renderer.hpp"

using namespace DirectX;

struct VSConstantBuffer
{
	XMFLOAT4X4 model;
	XMFLOAT4X4 view;
	XMFLOAT4X4 projection;
	XMFLOAT3 viewPos;
	float roughness;
	float p1, p2, p3, p4;
};

struct MaterialConstantBuffer
{
	XMFLOAT3 albedo;
	float metallic;
	float roughness;
	XMFLOAT3 emissive;
	float ao;
	bool maps;
	float p2, p3;
};


Renderer::Renderer()
{
	// Shader layouts
	D3D11_INPUT_ELEMENT_DESC normalDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC positionDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC positionUVDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC colorDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Init device, window, swapchain
	Shader cubemapShader(device, L"cubemap.hlsl", positionDesc, ARRAYSIZE(positionDesc));

	VSConstantBuffer cb{};
	cb.roughness = 0.5f;
	ID3D11Buffer* vsCb = device.createBuffer(&cb, sizeof(VSConstantBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);

	MaterialConstantBuffer material{};
	material.albedo = XMFLOAT3(1.0f, 0.0f, 0.0f);
	material.metallic = 0.0f;
	material.roughness = 0.5f;
	material.ao = 1.0f;
	material.emissive = XMFLOAT3(0.0f, 0.0f, 0.0f);
	material.maps = true;
	ID3D11Buffer* materialCb = device.createBuffer(&material, sizeof(MaterialConstantBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);

}

void Renderer::render(entt::registry& registry, RenderWindow& window)
{
}
