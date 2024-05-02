#include "Texture.fxh"

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PS_INPUT input) : SV_Target
{
    float3 finalColor = float3(0.f, 0.f, 0.f);
    float4 baseColor = float4(0.f, 1.f, 0.f, 1.f);
    float3 Normal = input.NorWorld;
    float Roughness = float(1.f);
    float Alpha = float(1.f);
    float3 EmissiveColor = float3(0.f, 0.f, 0.f);

    //Diffuse mapping
    if(HasDiffuseMap)
    {
        baseColor = txDiffuse.Sample(samLinear, input.TexCoord);
    }
    //normalmapping
    if(HasNormalMap)
    {
        float3 TextureSpace = txNormal.Sample(samLinear, input.TexCoord).rgb * 2.f - 1.f;
        input.NorWorld = normalize(input.NorWorld);
        input.TanWorld = normalize(input.TanWorld);
        input.BitWorld = normalize(input.BitWorld);
        float3x3 TextureTransform = float3x3(input.TanWorld, input.BitWorld, input.NorWorld);
        Normal = normalize(mul(TextureSpace, TextureTransform));
    }

    //roughnessmapping
    if(HasSpecularMap)
    {
        Roughness = txSpecular.Sample(samLinear, input.TexCoord);
    }

    //OpacityMapping
    if (HasOpacityMap)
    {
       Alpha = txOpacity.Sample(samLinear, input.TexCoord).a;
    }

    //texturemapping
    float3 lightDirection = normalize(lightDir.xyz);

    //ambient
    float3 ambientColor = lightAmbient.rgb * baseColor.rgb;
    //diffuse
    float diff = saturate(dot(-Normal, lightDirection));
    float3 diffuseColor = baseColor.rgb * (diff * lightDiffuse.rgb);
    //specular
    float3 viewDir = normalize(viewPosition - input.PosWorld);
    float3 halfVector = normalize(-lightDirection + viewDir);
    float HDotN = max(0.f, dot(halfVector, Normal));
    float3 specularColor = pow(HDotN, 2000) * lightSpecular.rgb * Roughness;

    finalColor.rgb = saturate(ambientColor + diffuseColor + specularColor);

    //EmissiveMapping
    if (HasEmissiveMap)
    {
        EmissiveColor = txEmissive.Sample(samLinear, input.TexCoord).rgb;
        finalColor.rgb += EmissiveColor;
    }

    return float4(finalColor, Alpha);
}