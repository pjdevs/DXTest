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
    
    output.direction = input.position;
    output.position = mul(mul(float4(input.position, 1.0), view), projection);

    return output;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{
    const float PI = 3.14159265359;
    float3 normal = normalize(input.direction);
    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));
    
    float3 irradiance = float3(0.0, 0.0, 0.0);

    float sampleDelta = 0.01;
    float nrSamples = 0.0;
    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += tex.Sample(textureSampler, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    
    return float4(irradiance, 1.0);
}