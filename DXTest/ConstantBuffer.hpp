#pragma once

#include "Graphics.hpp"

template <typename T>
class ConstantBuffer
{
private:
	ID3D11Buffer* _cb;
	D3D11_MAPPED_SUBRESOURCE _cbMappedSubResource;

public:
	T data;

public:
	ConstantBuffer(const Graphics& device);
	ConstantBuffer(const Graphics& device, const T& initialData);
	~ConstantBuffer();

	void upload(const Graphics& device);
	void setPS(const Graphics& device, int slot);
	void setVS(const Graphics& device, int slot);

};

template<typename T>
inline ConstantBuffer<T>::ConstantBuffer(const Graphics& device, const T& initialData)
	: _cb(nullptr), data(initialData)
{
	_cb = device.createBuffer(&data, sizeof(T), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE);
}

template<typename T>
inline ConstantBuffer<T>::ConstantBuffer(const Graphics& device)
	: ConstantBuffer<T>::ConstantBuffer(device, {})
{
}

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
	_cb->Release();
}

template<typename T>
inline void ConstantBuffer<T>::upload(const Graphics& device)
{
	device.getDeviceContext()->Map(_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &_cbMappedSubResource);
	memcpy(_cbMappedSubResource.pData, &data, sizeof(T));
	device.getDeviceContext()->Unmap(_cb, 0);
}

template<typename T>
inline void ConstantBuffer<T>::setPS(const Graphics& device, int slot)
{
	device.getDeviceContext()->PSSetConstantBuffers(slot, 1, &_cb);

}

template<typename T>
inline void ConstantBuffer<T>::setVS(const Graphics& device, int slot)
{
	device.getDeviceContext()->VSSetConstantBuffers(slot, 1, &_cb);
}
