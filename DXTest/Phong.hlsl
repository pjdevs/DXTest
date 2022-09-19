Texture2D tex : register(t0);
SamplerState textureSampler : register(s0);

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
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float3 worldPos : POSITION;
    float2 uv : TEXCOORD0;
     float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.position, 1.0), model);
    
    output.worldPos = worldPos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.position = mul(mul(worldPos, view), projection);

    return output;
}

float4 PSMain(linear VS_OUT input) : SV_TARGET
{
    const float3 lightDir = float3(-1.0, -0.5, -1.0);
    const float3 color = tex.Sample(textureSampler, input.uv);
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(viewPos - input.worldPos);
    float3 reflectedLightDir = reflect(lightDir, normal);

    float3 ambient = 0.1 * color;
    float3 diffuse = saturate(dot(normal, -lightDir)) * color;
    float3 specular = pow(saturate(dot(viewDir, reflectedLightDir)), 32) * color;
    float3 result = ambient + diffuse + specular;
    
    result /= (result + 1.0);
    
    return float4(result, 1.0f);
}