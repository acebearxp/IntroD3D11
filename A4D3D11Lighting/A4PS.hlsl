#include "shared.hlsli"

float4 main(VS_OUTPUT input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float3 toEyeW = normalize(v4EyePos.xyz - input.PosW);
    
    // Direction Light
    float4 ambientDir = gDirLight.Ambient * gMaterial.Ambient;
    float4 diffuseDir = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specularDir = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightDirVec = -gDirLight.Direction;
    float diffuseDirFactor = dot(lightDirVec, normal);
    
    [flatten]
    if (diffuseDirFactor > 0.0f)
    {
        float3 v = reflect(gDirLight.Direction, normal);
        float specFactor = max(dot(v, toEyeW), 0.0f);
        
        diffuseDir = gDirLight.Diffuse * gMaterial.Diffuse * diffuseDirFactor;
        specularDir = gDirLight.Specular * gMaterial.Specular * pow(specFactor, gMaterial.Specular.w);
    }
    float4 color = ambientDir + diffuseDir + specularDir;
    
    // Point Light
    float4 ambientPoint = gPointLight.Ambient * gMaterial.Ambient;
    float4 diffusePoint = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specularPoint = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    float3 lightPointVec = gPointLight.Pos - input.PosW;
    float lightPointDist = length(lightPointVec);
    lightPointVec /= lightPointDist; // normalize
    float diffusePointFactor = dot(lightPointVec, normal);
    float att = 1.0f / dot(gPointLight.Attenuation, float4(1.0f, lightPointDist, lightPointDist * lightPointDist, 0.0f));
    
    [flatten]
    if(lightPointDist < gPointLight.Range && diffusePointFactor > 0.0f)
    {
        float3 v = reflect(-lightPointVec, normal);
        float specFactor2 = max(dot(v, toEyeW), 0.0f);
        
        diffusePoint = gPointLight.Diffuse * gMaterial.Diffuse * diffusePointFactor;
        specularPoint = gPointLight.Specular * gMaterial.Specular * pow(specFactor2, gMaterial.Specular.w);
    }
    color +=  ambientPoint + (diffusePoint + specularPoint)*att;
    
    color.a = gMaterial.Diffuse.a;
    color = saturate(color);
    
    return color;
}