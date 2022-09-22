#pragma once

#include <exception>
#include <d3d11.h>
#include "GraphicsDevice.hpp"

//class Texture
//{
//private:
//	ID3D11Texture2D* _texture;
//	ID3D11ShaderResourceView* _textureView;
//	ID3D11SamplerState* _sampler;
//	ID3D11RenderTargetView* _rtView;
//
//public:
//	Texture(const GraphicsDevice& device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format, bool renderable = false);
//	~Texture();
//
//	ID3D11ShaderResourceView* getTextureView();
//	ID3D11SamplerState* getSampler();
//	ID3D11RenderTargetView* getRTView();
//
//	void bind(const GraphicsDevice& device, int slot);
//};

class Texture
{
private:
	D3D11_TEXTURE2D_DESC _desc;
	ID3D11Texture2D* _texture;
	ID3D11ShaderResourceView* _textureView;
	ID3D11SamplerState* _sampler;

public:
	Texture(const GraphicsDevice& device, int width, int height, UINT pixelSize, int mipLevels, int arraySize, DXGI_FORMAT format, D3D11_USAGE usage, UINT bindFlags, UINT miscFlags, D3D11_SRV_DIMENSION viewDimension, void* data);
	~Texture();

	ID3D11ShaderResourceView* getTextureView();
	ID3D11RenderTargetView* getRenderTargetView(const GraphicsDevice& device, int face = 0, int mip = 0, int size = 1);
	ID3D11SamplerState* getSampler();

	void bind(const GraphicsDevice& device, int slot);
};