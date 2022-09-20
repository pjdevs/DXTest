#pragma once

#include <exception>
#include <d3d11.h>

class Texture
{
private:
	ID3D11Texture2D* _texture;
	ID3D11ShaderResourceView* _textureView;
	ID3D11SamplerState* _sampler;
	ID3D11RenderTargetView* _rtView;

public:
	Texture(ID3D11Device* device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format, bool renderable = false);
	~Texture();

	ID3D11ShaderResourceView* getTextureView();
	ID3D11SamplerState* getSampler();
	ID3D11RenderTargetView* getRTView();
};

class TextureCube
{
private:
	ID3D11Texture2D* _texture;
	ID3D11ShaderResourceView* _textureView;
	ID3D11SamplerState* _sampler;
	ID3D11RenderTargetView* _facesRTView[6];

public:
	TextureCube(ID3D11Device* device, int width, int height, int mipLevels);
	~TextureCube();

	ID3D11ShaderResourceView* getTextureCubeView();
	ID3D11RenderTargetView* getCubeFaceRTView(int face);
	ID3D11SamplerState* getSampler();
};