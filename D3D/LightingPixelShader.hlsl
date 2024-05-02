#include "Lighting.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float4 finalColor = float4(0, 0, 0, 0);

    float3 lightDir = normalize(vLightDir.xyz);
    float diff = saturate(dot(input.Norm, lightDir));
    finalColor.rgb = input.Color.rgb * (diff * vLightColor.rgb);

    finalColor.a = 1;
    return finalColor;
}