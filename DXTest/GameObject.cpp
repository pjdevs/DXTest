#include "GameObject.hpp"

using namespace DirectX;

static void setTransformDirty(entt::registry& registry, entt::entity entity)
{
	registry.get<TransformComponent>(entity).dirty = true;
}

GameObject::GameObject(Scene& scene)
	: registry(scene.registry), entity(scene.registry.create())
{
	registry.emplace<TransformComponent>(entity);
	registry.emplace<HierarchyComponent>(entity);
	registry.on_update<TransformComponent>().connect<&setTransformDirty>();
}

TransformComponent::TransformComponent()
{
	dirty = false;
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4x4(&world, XMMatrixIdentity());
}

CameraComponent::CameraComponent()
	: main(false), camera(0.0f, 0.0f, 0.0f)
{

}

void TransformComponent::update()
{
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMMatrixAffineTransformation(
		XMLoadFloat3(&scale),
		XMLoadFloat3(&position),
		XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)),
		XMLoadFloat3(&position)
	)));
}

HierarchyComponent::HierarchyComponent()
{
	parent = entt::null;
	child = entt::null;
	nextSibling = entt::null;
	prevSibling = entt::null;
}
