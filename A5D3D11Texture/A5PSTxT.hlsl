#include "shared.hlsli"

Texture2D gTex: register(t0);
Texture2D gTexAlpha: register(t1);
SamplerState gSampler: register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
    float theta = -2.0f * 2.0f * 3.1415926f * vTmDelta.r; // speed * 2PI * seconds
    float2x2 mxTex = float2x2(cos(theta), -sin(theta), sin(theta), cos(theta));
    float2 texCoord = mul((input.Tex - float2(0.5f, 0.5f)), mxTex) + float2(0.5f, 0.5f);
    
    float4 texColor = gTex.Sample(gSampler, texCoord);
    float4 texAlpha = gTexAlpha.Sample(gSampler, texCoord);
    
    float4 output = texColor * texAlpha;
    return output;
}