#pragma once

#include <DirectXMath.h>
#include <memory>
#include "Shader.hpp"
#include "Texture.hpp"

struct Material
{
	std::shared_ptr<Shader> shader;

	DirectX::XMFLOAT3 albedo;
	float metallic;
	float roughness;
	DirectX::XMFLOAT3 emissive;
	float ao;

	std::shared_ptr<Texture> albedoMap;
	std::shared_ptr<Texture> emissiveMap;
	std::shared_ptr<Texture> metallicRoughnessMap;
	std::shared_ptr<Texture> aoMap;
};