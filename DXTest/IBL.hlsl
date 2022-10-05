SamplerState defaultSampler : register(s0);

TextureCube envMap : register(t0);
TextureCube irradianceMap : register(t1);
TextureCube preFilterMap : register(t2);
Texture2D brdfLUT : register(t3);

Texture2D albedoMap : register(t4);
Texture2D metallicRoughnessMap : register(t5);
Texture2D normalMap : register(t6);
Texture2D aoMap : register(t7);
Texture2D emissiveMap : register(t8);

cbuffer VSConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float3 viewPos;
};

cbuffer MaterialConstantBuffer : register(b1)
{
    float3 materialAlbedo;
    float materialMetallic;
    float materialRoughness;
    float3 materialEmissive;
    float materialAO;
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
    return F0 + (max(float3(smoothness, smoothness, smoothness), F0) - F0) * pow(saturate(1.0 - cosTheta), 5.0);
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    // Material
    float3 albedo = materialAlbedo;
    float metallic = materialMetallic;
    float roughness = materialRoughness;
    float3 normal = normalize(input.normal);
    float3 ao = materialAO;
    float3 emissive = materialEmissive;
    
    // Sample maps
    float3 metallicRoughness = metallicRoughnessMap.Sample(defaultSampler, input.uv);;
    albedo = albedoMap.Sample(defaultSampler, input.uv);
    metallic = metallicRoughness.b;
    roughness = metallicRoughness.g;
    //normal *= normalMap.Sample(defaultSampler, input.uv);
    ao = aoMap.Sample(defaultSampler, input.uv);
    emissive = emissiveMap.Sample(defaultSampler, input.uv);

    // Setup usefull vectors
    float3 viewDir = normalize(viewPos - input.worldPos);

    // IBL
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);
    float3 F = FresnelSchlickRoughness(saturate(dot(normal, viewDir)), F0, roughness);
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metallic);
    
    // Diffuse IBL
    float3 irradiance = irradianceMap.Sample(defaultSampler, normal).rgb;
    float3 diffuse = irradiance * albedo;
    
    // Specular IBL
    float3 R = reflect(-viewDir, normal);
    const float MAX_REFLECTION_LOD = 4.0;
    float3 prefilteredColor = preFilterMap.SampleLevel(defaultSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    float2 envBRDF = brdfLUT.Sample(defaultSampler, float2(saturate(dot(normal, viewDir)), roughness)).rg;
    float3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
  
    // Final color
    float3 ambient = (kD * diffuse + specular) * ao + emissive;
    float3 result = ambient;
    
    // Tonemapping
    //result = 1.0 - exp(-result * 1.0);
    // Gamma correction
    result = pow(result, 1.0 / 2.2);
    
    return float4(result, 1.0f);
}