#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	virtual const DirectX::XMFLOAT3& getPosition() const = 0;
	virtual const DirectX::XMFLOAT4X4& getView() const = 0;
	virtual const DirectX::XMFLOAT4X4& getProjection() const = 0;
	virtual void setSize(float width, float height) = 0;
	virtual void update(float mouseDx, float mouseDy, float mouseScrollY, bool mouseRight, float dt) = 0;
	virtual void computeMatrices() = 0;
};

class SphericalCamera : public Camera
{
private:
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT3 _target;
	float _distance;
	float _phi;
	float _theta;
	float _width, _height;
	DirectX::XMFLOAT4X4 _view;
	DirectX::XMFLOAT4X4 _projection;
public:
	SphericalCamera(float targetX, float targetY, float targetZ);

	const DirectX::XMFLOAT3& getPosition() const override;
	const DirectX::XMFLOAT4X4& getView() const override;
	const DirectX::XMFLOAT4X4& getProjection() const override;
	void setSize(float width, float height) override;
	void update(float mouseDx, float mouseDy, float mouseScrollY, bool mouseRight, float dt) override;
	void computeMatrices() override;
};

class CubeCamera : public Camera
{
private:
	DirectX::XMFLOAT3 _position;
	DirectX::XMFLOAT4X4 _view;
	DirectX::XMFLOAT4X4 _projection;

	static DirectX::XMMATRIX _viewMatrices[6];

public:
	CubeCamera();

	const DirectX::XMFLOAT3& getPosition() const override;
	const DirectX::XMFLOAT4X4& getView() const override;
	const DirectX::XMFLOAT4X4& getProjection() const override;
	void setSize(float width, float height) override;
	void update(float mouseDx, float mouseDy, float mouseScrollY, bool mouseRight, float dt) override;
	void computeMatrices() override;
	void face(int face);
};
