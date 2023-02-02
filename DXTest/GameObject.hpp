#pragma once

#include <entt/entity/registry.hpp>
#include <DirectXMath.h>
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

struct TransformComponent
{
	bool dirty;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4X4 world;

	TransformComponent();
	void update();
};

struct HierarchyComponent
{
	entt::entity parent;
	entt::entity child;
	entt::entity nextSibling;
	entt::entity prevSibling;

	HierarchyComponent();
};

struct MeshComponent
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Shader> shader;
};

struct CameraComponent
{
	bool main;
	SphericalCamera camera;

	CameraComponent();
};

class Scene
{
public:
	entt::registry registry;
};

class GameObject
{
private:
	entt::entity entity;
	entt::registry& registry;

public:
	GameObject(Scene& scene);

	template<class T> 
	void addComponent()
	{
		registry.emplace<T>(entity);
	}

	template<class T>
	void removeComponent()
	{
		registry.remove<T>(entity);
	}

	template<class T>
	T& getComponent()
	{
		return registry.get<T>(entity);
	}
};