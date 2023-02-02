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
    bool materialMaps;
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
    float3 normal : NORMAL;
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

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(saturate(1.0 - cosTheta), 5.0);
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
    if (materialMaps)
    {
        float3 metallicRoughness = metallicRoughnessMap.Sample(defaultSampler, input.uv);
        albedo = albedoMap.Sample(defaultSampler, input.uv);
        metallic = metallicRoughness.b;
        roughness = metallicRoughness.g;
        //normal *= normalMap.Sample(defaultSampler, input.uv);
        ao = aoMap.Sample(defaultSampler, input.uv);
        emissive = emissiveMap.Sample(defaultSampler, input.uv);
    }
    
    // Setup usefull vectors
    float3 viewDir = normalize(viewPos - input.worldPos);

    // IBL
    float3 F0 = 0.04;
    F0 = lerp(F0, albedo, metallic);
    float3 F = FresnelSchlick(saturate(dot(normal, viewDir)), F0);
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
    
    float3 ambient = (kD * diffuse + specular) * ao + emissive;
    
    // Direct lighting
    const float3 lightPosition = float3(1.0, 1.0, 3.0);
    const float3 lightColor = 20.0;
    float3 Lo = 0.0;
    
    float3 L = normalize(lightPosition - input.worldPos);
    float3 H = normalize(viewDir + L);
    float distance = length(lightPosition - input.worldPos);
    float attenuation = 1.0; // (distance * distance);
    float3 radiance = lightColor * attenuation;
        
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, viewDir, L, roughness);
    F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);
        
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;
        
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    specular = numerator / denominator;
            
    // add to outgoing radiance Lo
    float NdotL = max(dot(normal, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    
    // Final color
    float3 result = Lo + ambient;
    //float3 result = (normal + 1.0) * 0.5;
    
    // Tonemapping
    result = 1.0 - exp(-result * 1.0);

    // Gamma correction
    result = pow(result, 1.0 / 2.2);
    
    return float4(result, 1.0f);
}