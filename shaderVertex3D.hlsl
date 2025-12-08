cbuffer MatrixBuffer : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
};

cbuffer Light : register(b1)
{
    bool lightEnable;
    float3 direction;
};

float4 main(in float4 position : POSITION0,
            in float2 tex : TEXCOORD0,
            in float4 color : COLOR0,
            in float3 normal : NORMAL0,
            out float2 outTex : TEXCOORD0,
            out float4 outColor : COLOR0) : SV_Position
{
    if (lightEnable)
    {
        float3 lightDir = direction;
        float3 nor = mul(float4(normal, 0.0f), world);
        nor = normalize(nor);
    
        // ÉâÉìÉoÅ[ÉgägéU
        outColor.rgb = saturate(-dot(lightDir, nor)) * color;
        outColor.a = 1.0f;
    
        // ä¬ã´åı
        outColor.rgb += float3(0.4f, 0.4f, 0.4f);
    }
    else
    {
        outColor = color;
    }
    
    
    outTex = tex;
    
    position = mul(position, world);
    position = mul(position, view);
    position = mul(position, projection);
    
    return position;
}