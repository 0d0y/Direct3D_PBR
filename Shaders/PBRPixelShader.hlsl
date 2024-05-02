#include "Texture.fxh"

static const float PI = 3.141592;
static const float3 Fdielectric = 0.04;

float Distribution_GGX(float NdotH, float a2)
{
    float a22 = a2 * a2;
    float d = (NdotH * a22 - NdotH) * NdotH + 1;
    return a22 / (PI * d * d);

    // D, 법선 분포 함수
    //float alphaSq = a2 * a2;

    // 디즈니 방식, alpha = roughness^2
    //float denom = NdotH * NdotH * (alphaSq - 1.0) + 1.0;
    //return alphaSq / (PI * denom * denom);
}

float3 Fresnel_Schlick(float VdotH, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);
}

//언리얼 방식
float3 Fresnel_Schlick(float3 SpecularColor, float VdotH)
{
    float Fc = pow(1 - VdotH, 5);
    return saturate(50.0 * SpecularColor.g) * Fc + (1 - Fc) * SpecularColor;
}

//언리얼 방식
float Geometric_Schlick(float a2, float NdotV, float NdotL)
{
    float k = sqrt(a2) * 0.5;
    float Vis_SchlickV = NdotV * (1 - k) + k;
    float Vis_SchlickL = NdotL * (1 - k) + k;
    return 0.25 / (Vis_SchlickV * Vis_SchlickL);
}

float Geometric_SmithJointApprox(float a2, float NoV, float NoL)
{
    float a = sqrt(a2);
    float Vis_SmithV = NoL * (NoV * (1 - a) + a);
    float Vis_SmithL = NoV * (NoL * (1 - a) + a);
    return 0.5 * rcp(Vis_SmithV + Vis_SmithL);
}

float gaSchlickG1(float cosTheta, float k)
{
    return (cosTheta / (cosTheta * (1 - k) + k));
}

float gaSchlickGGX(float cosNL, float cosNV, float roughness)
{
    // G, 기하학적 감쇠
    float alpha = roughness + 1.0;
    float k = (alpha * alpha) / 8.0; // Epic
    return gaSchlickG1(cosNL, k) * gaSchlickG1(cosNV, k);
}

float3 CookTorrance_SpecularBRDF(float3 F, float D, float G, float NdotL, float NdotV)
{
    return (D * F * G) / max(1e-5, 4.0 * NdotL * NdotV);
}

float3 Lambertian_DiffuseBRDF(float3 albedo)
{
    return albedo / PI;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 albedo = float3(0.f, 1.f, 0.f);
    if (HasDiffuseMap)
    {
        albedo = txDiffuse.Sample(samLinear, input.TexCoord).rgb;
    }
    //Gamma Correction
    albedo = pow(albedo, 2.2);

    float3 N = input.NorWorld;
    if (HasNormalMap)
    {
        float3 TextureSpace = txNormal.Sample(samLinear, input.TexCoord).rgb * 2.f - 1.f;
        input.NorWorld = normalize(input.NorWorld);
        input.TanWorld = normalize(input.TanWorld);
        input.BitWorld = normalize(input.BitWorld);
        float3x3 TextureTransform = float3x3(input.TanWorld, input.BitWorld, input.NorWorld);
        N = normalize(mul(TextureSpace, TextureTransform));
    }
    float metalness = 0.f;
    if (HasMetalnessMap)
    {
        metalness = txMetalness.Sample(samLinear, input.TexCoord).r;
    }
    float roughness = 0.f;
    if (HasRoughnessMap)
    {
        roughness = txRoughness.Sample(samLinear, input.TexCoord).r;
    }

    //ambient
    //float3 ambientColor = lightAmbient.rgb * albedo;

    float3 lightDirection = normalize(lightDir.xyz);
    float3 viewDir = normalize(viewPosition - input.PosWorld);
    float3 halfVector = normalize(-lightDirection + viewDir);

    float NdotH = max(0.f, dot(N, halfVector));
    float VdotH = max(0.f, dot(viewDir, halfVector));
    float NdotV = max(0.f, dot(N, viewDir));
    float NdotL = max(0.f, dot(N, -lightDirection));

    float a2 = roughness * roughness;
    //DFG
    float D = Distribution_GGX(NdotH, a2);

    //float3 SpecularColor = lerp(0.08 * lightSpecular.xxx, albedo, metalness);
    //float3 F = Fresnel_Schlick(SpecularColor, VdotH);
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    float3 F = Fresnel_Schlick(VdotH, F0);

    float G = gaSchlickGGX(NdotL, NdotV, roughness);
    //float G = Geometric_Schlick(a2, NdotV, NdotL);
    //float G = Geometric_SmithJointApprox(a2, NdotV, NdotL);

    float3 specularBRDF = CookTorrance_SpecularBRDF(F, D, G, NdotL, NdotV);
    // diffuseBRDF = Lambertian_DiffuseBRDF(albedo);
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
    float3 diffuseBRDF = kd * Lambertian_DiffuseBRDF(albedo);

    //float3 finalColor = (diffuseBRDF + specularBRDF) * NdotL;
    float3 finalColor = (diffuseBRDF + specularBRDF) * lightSpecular.rgb * NdotL;
    finalColor = pow(finalColor, 1 / 2.2);

    return float4(finalColor, 1.0f);

}