#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "Graphics.hpp"

class Shader
{
private:
	std::wstring _path;
	D3D11_INPUT_ELEMENT_DESC* _layoutDesc;
	int _layoutSize;

	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;

private:
	void loadShader(const Graphics& device);
	void unloadShader();

	static HRESULT compileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

public:
	Shader(const Graphics& device, const std::wstring& shaderPath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, int layoutSize);
	~Shader();

	ID3D11VertexShader* getVertexShader() const;
	ID3D11PixelShader* getPixelShader() const;
	ID3D11InputLayout* getInputLayout() const;

	void reload(const Graphics& device);

	void bind(const Graphics& device);
};

