Texture2D g_tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(in float4 position : SV_Position,
            in float2 tex : TEXCOORD0,
            in float4 col : COLOR0) : SV_Target0
{
    return col * g_tex.Sample(samplerState, tex);
}