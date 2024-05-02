#include "Texture.fxh"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.PosWorld = mul(float4(input.PosVertex, 1), World);
    output.PosProj = mul(float4(output.PosWorld, 1.f), View);
    output.PosProj = mul(output.PosProj, Projection);
    output.NorWorld = normalize(mul(input.NorVertex, (float3x3) World));
    output.TanWorld = normalize(mul(input.TanVertex, (float3x3) World));
    //output.BitWorld = cross(output.NorWorld, output.TanWorld);
    output.BitWorld = normalize(mul(input.BitVertex, (float3x3) World));
    output.TexCoord = input.TexCoord;

    return output;
}