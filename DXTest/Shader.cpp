#include "Shader.hpp"

Shader::Shader(const GraphicsDevice& device, const std::wstring& shaderPath)
{
	// Creating shaders
	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	hr = compileShader(shaderPath.c_str(), "VSMain", "vs_5_0", &vsBlob);
	if (FAILED(hr))
		throw new std::exception("Error while compiling vertex shader");

	hr = compileShader(shaderPath.c_str(), "PSMain", "ps_5_0", &psBlob);
	if (FAILED(hr))
		throw new std::exception("Error while compiling pixel shader");

	hr = device.getDevice()->CreateVertexShader(
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		nullptr,
		&_vertexShader
	);
	if (FAILED(hr))
		throw new std::exception("Error while creating vertex shader");

	hr = device.getDevice()->CreatePixelShader(
		psBlob->GetBufferPointer(),
		psBlob->GetBufferSize(),
		nullptr,
		&_pixelShader
	);
	if (FAILED(hr))
		throw new std::exception("Error while creating pixel shader");

	// Input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = device.getDevice()->CreateInputLayout(
		inputElementDesc,
		2,
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		&_inputLayout
	);

	if (FAILED(hr))
		throw new std::exception("Error while creating input layer");

	// Release shader compiled code
	vsBlob->Release();
	psBlob->Release();
}

Shader::~Shader()
{
	_inputLayout->Release();
	_pixelShader->Release();
	_vertexShader->Release();
}

ID3D11VertexShader* Shader::getVertexShader() const
{
	return _vertexShader;
}

ID3D11PixelShader* Shader::getPixelShader() const
{
	return _pixelShader;
}

ID3D11InputLayout* Shader::getInputLayout() const
{
	return _inputLayout;
}

HRESULT Shader::compileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}
