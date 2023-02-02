#pragma once

#include <DirectXMath.h>
#include <entt/entity/registry.hpp>
#include "RenderWindow.hpp"
#include "Shader.hpp"

struct RendererSettings
{
	DirectX::XMFLOAT3 clearColor;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	float mouseScrollY;
	bool drawTriedre;
	bool showIrradiance;
	bool showPreFilter;
	bool drawSpheres;
};

class Renderer
{
private:
	Graphics device;
	CD3D11_VIEWPORT viewport;
	D3D11_MAPPED_SUBRESOURCE cbRessource;

public:
	RendererSettings settings;

	Renderer();
	void render(entt::registry& registry, RenderWindow& window);
};

