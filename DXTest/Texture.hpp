#pragma once

#include <exception>
#include <d3d11.h>

class Texture
{
private:
	ID3D11Texture2D* _texture;
	ID3D11ShaderResourceView* _textureView;
	ID3D11SamplerState* _sampler;

public:
	Texture(ID3D11Device* device, int width, int height, void* data, UINT pixelSize, DXGI_FORMAT format);
	~Texture();

	ID3D11ShaderResourceView* getTextureView();
	ID3D11SamplerState* getSampler();
};

