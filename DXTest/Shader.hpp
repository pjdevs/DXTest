#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include "GraphicsDevice.hpp"

class Shader
{
private:
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;

private:
	static HRESULT compileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);

public:
	Shader(const GraphicsDevice& device, const std::wstring& shaderPath);
	~Shader();

	ID3D11VertexShader* getVertexShader() const;
	ID3D11PixelShader* getPixelShader() const;
	ID3D11InputLayout* getInputLayout() const;
};

