#include "Texture.hpp"

Texture::Texture(ID3D11Device* device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format)
	: _texture(nullptr), _textureView(nullptr), _sampler(nullptr)
{
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	D3D11_SUBRESOURCE_DATA textureData = { 0 };
	textureData.pSysMem = data;
	textureData.SysMemPitch = pixelSize * width;

	HRESULT hr = device->CreateTexture2D(&textureDesc, &textureData, &_texture);

	if (FAILED(hr))
		throw std::exception("Cannot create texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(_texture, &textureViewDesc, &_textureView);

	if (FAILED(hr))
		throw std::exception("Cannot create resource view");

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

	hr = device->CreateSamplerState(&samplerDesc, &_sampler);

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

ID3D11SamplerState* Texture::getSampler()
{
	return _sampler;
}
