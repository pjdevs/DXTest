TextureCube tex : register(t0);
SamplerState textureSampler : register(s0);

cbuffer VSConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VS_IN
{
    float3 position : POSITION;
};

struct VS_OUT
{
    float3 direction : POSITION;
    float4 position : SV_POSITION;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    matrix viewUntranslated = view;
    viewUntranslated._m33 = 0.0;
    viewUntranslated._m32 = 0.0;
    viewUntranslated._m31 = 0.0;
    
    output.direction = input.position;
    output.position = mul(mul(float4(input.position, 1.0), viewUntranslated), projection);
    output.position = output.position.xyww;

    return output;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    float3 result = tex.Sample(textureSampler, input.direction);
    result /= (result + 1.0);
    result = pow(result, 1.0/2.2);
    
    return float4(result, 1.0); 
}