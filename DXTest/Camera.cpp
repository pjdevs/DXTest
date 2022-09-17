#include "Camera.hpp"

using namespace DirectX;

SphericalCamera::SphericalCamera(float targetX, float targetY, float targetZ)
	: _target(targetX, targetY, targetZ), _position(0.0f, 0.0f, 0.0f),
	_distance(3.0f), _phi(0.0f), _theta(-M_PI * 0.33f),
	_width(16.0f), _height(9.0f)
{
	computeMatrices();
}

void SphericalCamera::computeMatrices()
{
	_position = XMFLOAT3(
		_distance * XMScalarCos(_phi) * XMScalarSin(_theta),
		_distance * XMScalarCos(_theta),
		_distance * XMScalarSin(_phi) * XMScalarSin(_theta)
	);

	XMStoreFloat4x4(
		&_view,
		XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(_position.x, _position.y, _position.z, 1.0f), XMVectorSet(_target.x, _target.y, _target.z, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f))));
	XMStoreFloat4x4(
		&_projection,
		XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(80.f), _width / _height, 0.1f, 100.f))
	);
	
}

const XMFLOAT3& SphericalCamera::getPosition() const
{
	return _position;
}

const XMFLOAT4X4& SphericalCamera::getView() const
{
	return _view;
}

const XMFLOAT4X4& SphericalCamera::getProjection() const
{
	return _projection;
}

void SphericalCamera::setSize(float width, float height)
{
	_width = width;
	_height = height;
}

void SphericalCamera::update(float mouseDx, float mouseDy, float mouseScrollY, bool mouseRight, float dt)
{
	_distance -= 25.0f * mouseScrollY * dt;

	if (mouseRight)
	{
		_phi += mouseDx * dt;
		_theta += mouseDy * dt;
	}
}