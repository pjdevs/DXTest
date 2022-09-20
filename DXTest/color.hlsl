cbuffer VSConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct VS_IN
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VS_OUT
{
    float3 color : COLOR;
    float4 position : SV_POSITION;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT output;
    output.color = input.color;
    output.position = mul(mul(mul(float4(input.position, 1.0), model), view), projection);

    return output;
}

float4 PSMain(VS_OUT input) : SV_TARGET
{   
    return float4(input.color, 1.0);
}