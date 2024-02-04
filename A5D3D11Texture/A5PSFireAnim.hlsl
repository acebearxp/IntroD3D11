#include "shared.hlsli"

Texture2DArray gTex120: register(t0);
SamplerState gSampler: register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal.xyz);
    
    //float3 vSunDir = mul((float3) gSun.vDir, (float3x3) mxWorld);
    
    float fSunFactor = max(dot(normal, -gSun.vDir.xyz), 0.0f);
    float4 sunDiffuse = gSun.vDiffuse * fSunFactor * input.Color;
    float4 sunAmbient = gSun.vAmbient;
    
    float4 texColor = gTex120.Sample(gSampler, float3(input.Tex, (60.0f * vTmDelta.r)%120.0f));
    float4 output = texColor * (sunDiffuse + sunAmbient);
    return output;
}