#include "Texture.hpp"

Texture::Texture(ID3D11Device* device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format, bool renderable)
	: _texture(nullptr), _textureView(nullptr), _sampler(nullptr), _rtView(nullptr)
{
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	if (renderable)
		textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
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

	if (renderable)
	{
		hr = device->CreateRenderTargetView(_texture, nullptr, &_rtView);

		if (FAILED(hr))
			throw std::exception("Cannot create render target view");
	}
}

Texture::~Texture()
{
	_rtView->Release();
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

ID3D11RenderTargetView* Texture::getRTView()
{
	return _rtView;
}




TextureCube::TextureCube(ID3D11Device* device, int width, int height, int mipLevels)
	: _texture(nullptr), _textureView(nullptr), _sampler(nullptr), _facesRTView()
{
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.MipLevels = mipLevels;
	textureDesc.ArraySize = 6;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &_texture);

	if (FAILED(hr))
		throw std::exception("Cannot create texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	textureViewDesc.Format = textureDesc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	textureViewDesc.TextureCube.MipLevels = textureDesc.MipLevels;
	textureViewDesc.TextureCube.MostDetailedMip = 0;

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

	for (int i = 0; i < 6; ++i)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc = CD3D11_RENDER_TARGET_VIEW_DESC(
			_texture,
			D3D11_RTV_DIMENSION_TEXTURE2D,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			i,
			1
		);
		hr = device->CreateRenderTargetView(_texture, &rtDesc, &_facesRTView[i]);

		if (FAILED(hr))
			throw std::exception("Cannot create render target view for cube face");
	}
}

TextureCube::~TextureCube()
{
	for (int i = 0; i < 6; ++i)
		_facesRTView[i]->Release();
	_sampler->Release();
	_textureView->Release();
	_texture->Release();
}

ID3D11ShaderResourceView* TextureCube::getTextureCubeView()
{
	return _textureView;
}

ID3D11RenderTargetView* TextureCube::getCubeFaceRTView(int face)
{
	return _facesRTView[face];
}

ID3D11SamplerState* TextureCube::getSampler()
{
	return _sampler;
}