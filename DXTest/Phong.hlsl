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
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 position : SV_POSITION;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.position, 1.0), model);
    
    output.worldPos = worldPos;
    output.normal = input.normal;
    output.position = mul(mul(worldPos, view), projection);

    return output;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    const float3 lightDir = float3(-1.0, -1.0, -1.0);
    const float3 color = float3(1.0, 0.1, 0.3);
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(input.worldPos - viewPos);
    float3 reflectedLightDir = reflect(lightDir, normal);
    
    float3 diffuse = max(dot(normal, -lightDir), 0.0) * color;
    float3 specular = pow(max(dot(viewDir, reflectedLightDir), 0.0), 32) * color;
    
    return float4(diffuse + specular, 1.0f);
}