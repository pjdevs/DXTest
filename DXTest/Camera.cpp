#include "Camera.hpp"

using namespace DirectX;

SphericalCamera::SphericalCamera(float targetX, float targetY, float targetZ)
	: _target(targetX, targetY, targetZ), _position(0.0f, 0.0f, 0.0f),
	_distance(3.0f), _phi(0.0f), _theta(XM_PI * 0.33f),
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
		XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(80.f), _width / _height, 0.1f, 1000.f))
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
		_theta -= mouseDy * dt;
	}

	if (_theta <= 0.01f)
		_theta = 0.01f;

	if (_theta >= XM_PI - 0.001f)
		_theta = XM_PI - 0.001f;
}

XMMATRIX CubeCamera::_viewMatrices[6] =
{
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f))),
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f))),
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f,1.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
	XMMatrixTranspose(XMMatrixLookAtRH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f), XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f))),
};

CubeCamera::CubeCamera()
	: _view(), _projection(), _position(0.0f, 0.0f, 0.0f)
{
	XMStoreFloat4x4(&_projection, XMMatrixTranspose(XMMatrixPerspectiveFovRH(XMConvertToRadians(90.f), 1.0f, 0.1f, 10.f)));
	face(0);
}

const XMFLOAT3& CubeCamera::getPosition() const
{
	return _position;
}

const XMFLOAT4X4& CubeCamera::getView() const
{
	return _view;
}

const XMFLOAT4X4& CubeCamera::getProjection() const
{
	return _projection;
}

void CubeCamera::setSize(float width, float height)
{
}

void CubeCamera::update(float mouseDx, float mouseDy, float mouseScrollY, bool mouseRight, float dt)
{
}

void CubeCamera::computeMatrices()
{
}


void CubeCamera::face(int face)
{
	XMStoreFloat4x4(&_view, _viewMatrices[face]);
}