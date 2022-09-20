Texture2D tex : register(t0);
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
    
    output.direction = input.position;
    output.position = mul(mul(float4(input.position, 1.0), view), projection);

    return output;
}

float2 SampleSphericalMap(float3 v)
{
    const float2 invAtan = float2(0.1591, 0.3183);
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;

    return uv;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    float2 uv = SampleSphericalMap(normalize(input.direction));
    float3 result = tex.Sample(textureSampler, uv);
    
    return float4(result, 1.0);
}