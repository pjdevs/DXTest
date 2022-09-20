TextureCube env : register(t0);
SamplerState envSampler : register(s0);

cbuffer VSConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float3 viewPos;
};

struct VS_IN
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float3 worldPos : POSITIONT;
    float2 uv : TEXCOORD;
    float3 normal : POSITION;
    float4 position : SV_POSITION;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.position, 1.0), model);
    
    output.worldPos = worldPos.xyz;
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 1.0), model).xyz;
    output.position = mul(mul(worldPos, view), projection);

    return output;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(viewPos - input.worldPos);
    float3 reflectedViewDir = reflect(-viewDir, normal);
    float3 light = env.Sample(envSampler, reflectedViewDir);
    float3 result = lerp(float3(0.8, 0.1, 0.1), light, 0.25);
    
    result /= (result + 1.0);
    
    return float4(result, 1.0f);
}