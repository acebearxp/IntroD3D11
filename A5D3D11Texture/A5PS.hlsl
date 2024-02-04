#include "shared.hlsli"

Texture2D gTex: register(t0);
SamplerState gSampler: register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal.xyz);
    float distToEye = length(vEyePos.xyz - input.PosW.xyz);
    
    float fSunFactor = max(dot(normal, -gSun.vDir.xyz), 0.0f);
    float4 sunDiffuse = gSun.vDiffuse * fSunFactor * input.Color;
    float4 sunAmbient = gSun.vAmbient;
    
    float4 texColor = gTex.Sample(gSampler, input.Tex);
    float4 output = texColor * (sunDiffuse + sunAmbient);
    // fog
    output = lerp(output, float4(vFog.rgb, 1.0f), saturate((-vFog.a + distToEye) / 100.0f));
    return output;
}