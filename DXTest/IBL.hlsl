TextureCube envMap : register(t0);
TextureCube irradianceMap : register(t1);
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

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    float smoothness = 1.0 - roughness;
    return F0 + (max(float3(smoothness, smoothness, smoothness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 viewDir = normalize(viewPos - input.worldPos);
    
    const float3 albedo = float3(0.8, 0.1, 0.1);
    const float roughness = 0.5;
    const float ao = 1.0;

    // Diffuse IBL
    const float3 F0 = float3(0.04, 0.04, 0.04);
    float3 kS = FresnelSchlickRoughness(saturate(dot(normal, viewDir)), F0, roughness);
    float3 kD = 1.0 - kS;
    float3 irradiance = irradianceMap.Sample(envSampler, normal).rgb;
    float3 diffuse = irradiance * albedo;
    float3 ambient = (kD * diffuse) * ao;
    
    // Trick for reflection
    float3 reflectedViewDir = reflect(-viewDir, normal);
    float3 reflection = envMap.Sample(envSampler, reflectedViewDir) * (1.0 - roughness);
    
    float3 result = lerp(ambient, reflection, (1.0 - roughness));
    result /= (result + 1.0);
    
    return float4(result, 1.0f);
}