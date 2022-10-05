#include "Texture.hpp"

//Texture::Texture(const GraphicsDevice& device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format, bool renderable)
//	: _texture(nullptr), _textureView(nullptr), _sampler(nullptr), _rtView(nullptr)
//{
//	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
//	textureDesc.Width = width;
//	textureDesc.Height = height;
//	textureDesc.Format = format;
//	textureDesc.Usage = D3D11_USAGE_DEFAULT;
//	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//	if (renderable)
//		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
//	textureDesc.SampleDesc.Count = 1;
//	textureDesc.MipLevels = 1;
//	textureDesc.ArraySize = 1;
//
//	D3D11_SUBRESOURCE_DATA textureData = { 0 };
//	textureData.pSysMem = data;
//	textureData.SysMemPitch = pixelSize * width;
//
//	HRESULT hr = device.getDevice()->CreateTexture2D(&textureDesc, &textureData, &_texture);
//
//	if (FAILED(hr))
//		throw std::exception("Cannot create texture");
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
//	textureViewDesc.Format = textureDesc.Format;
//	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
//	textureViewDesc.Texture2D.MostDetailedMip = 0;
//
//	hr = device.getDevice()->CreateShaderResourceView(_texture, &textureViewDesc, &_textureView);
//
//	if (FAILED(hr))
//		throw std::exception("Cannot create resource view");
//
//	D3D11_SAMPLER_DESC samplerDesc;
//	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.MipLODBias = 0.0f;
//	samplerDesc.MaxAnisotropy = 1;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//	samplerDesc.BorderColor[0] = 0;
//	samplerDesc.BorderColor[1] = 0;
//	samplerDesc.BorderColor[2] = 0;
//	samplerDesc.BorderColor[3] = 0;
//	samplerDesc.MinLOD = 0;
//	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//	hr = device.getDevice()->CreateSamplerState(&samplerDesc, &_sampler);
//
//	if (FAILED(hr))
//		throw std::exception("Cannot create sampler state");
//
//	if (renderable)
//	{
//		hr = device.getDevice()->CreateRenderTargetView(_texture, nullptr, &_rtView);
//
//		if (FAILED(hr))
//			throw std::exception("Cannot create render target view");
//	}
//}
//
//Texture::~Texture()
//{
//	if (_rtView)
//		_rtView->Release();
//	_sampler->Release();
//	_textureView->Release();
//	_texture->Release();
//}
//
//ID3D11ShaderResourceView* Texture::getTextureView()
//{
//	return _textureView;
//}
//
//ID3D11SamplerState* Texture::getSampler()
//{
//	return _sampler;
//}
//
//ID3D11RenderTargetView* Texture::getRTView()
//{
//	return _rtView;
//}
//
//void Texture::bind(const GraphicsDevice& device, int slot)
//{
//	device.getDeviceContext()->PSSetShaderResources(slot, 1, &_textureView);
//	device.getDeviceContext()->PSSetSamplers(slot, 1, &_sampler);
//}

Texture::Texture(const GraphicsDevice& device, int width, int height, UINT pixelSize, int mipLevels, int arraySize, DXGI_FORMAT format, D3D11_USAGE usage, UINT bindFlags, UINT miscFlags, D3D11_SRV_DIMENSION viewDimension, void* data)
	: _desc(), _texture(nullptr), _textureView(nullptr), _sampler(nullptr)
{
	_desc.Width = width;
	_desc.Height = height;
	_desc.Format = format;
	_desc.Usage = usage;
	_desc.BindFlags = bindFlags;
	_desc.SampleDesc.Count = 1;
	_desc.MipLevels = mipLevels;
	_desc.ArraySize = arraySize;
	_desc.MiscFlags = miscFlags;

	D3D11_SUBRESOURCE_DATA textureData = { 0 };
	textureData.pSysMem = data;
	textureData.SysMemPitch = pixelSize * width;

	HRESULT hr = device.getDevice()->CreateTexture2D(&_desc, data != nullptr ? &textureData : nullptr , &_texture);

	if (FAILED(hr))
		throw std::exception("Cannot create texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	textureViewDesc.Format = _desc.Format;
	textureViewDesc.ViewDimension = viewDimension;
	textureViewDesc.TextureCube.MipLevels = _desc.MipLevels;
	textureViewDesc.TextureCube.MostDetailedMip = 0;

	hr = device.getDevice()->CreateShaderResourceView(_texture, &textureViewDesc, &_textureView);

	if (FAILED(hr))
		throw std::exception("Cannot create resource view");

	if (mipLevels == 0 || mipLevels > 1)
		device.getDeviceContext()->GenerateMips(_textureView);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device.getDevice()->CreateSamplerState(&samplerDesc, &_sampler);

	if (FAILED(hr))
		throw std::exception("Cannot create sampler state");
}

Texture::~Texture()
{
	_sampler->Release();
	_textureView->Release();
	_texture->Release();
}

ID3D11ShaderResourceView* Texture::getTextureView()
{
	return _textureView;
}

ID3D11RenderTargetView* Texture::getRenderTargetView(const GraphicsDevice& device, int face, int mip, int size)
{
	ID3D11RenderTargetView* rtView = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC rtDesc = CD3D11_RENDER_TARGET_VIEW_DESC(
		_texture,
		D3D11_RTV_DIMENSION_TEXTURE2DARRAY,
		_desc.Format,
		mip,
		face,
		size
	);
	HRESULT hr = device.getDevice()->CreateRenderTargetView(_texture, &rtDesc, &rtView);

	if (FAILED(hr))
		throw std::exception("Cannot create render target view for cube face");

	return rtView;
}

ID3D11SamplerState* Texture::getSampler()
{
	return _sampler;
}

void Texture::bind(const GraphicsDevice& device, int slot)
{
	device.getDeviceContext()->PSSetShaderResources(slot, 1, &_textureView);
	device.getDeviceContext()->PSSetSamplers(slot, 1, &_sampler);
}