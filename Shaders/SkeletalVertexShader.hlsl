#include "Texture.fxh"

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4x4 matWorldBlended;
    matWorldBlended = mul(input.BlendWeights.x, MatrixPalleteArray[input.BlendIndices.x]);
    matWorldBlended += mul(input.BlendWeights.y, MatrixPalleteArray[input.BlendIndices.y]);
    matWorldBlended += mul(input.BlendWeights.z, MatrixPalleteArray[input.BlendIndices.z]);
    matWorldBlended += mul(input.BlendWeights.w, MatrixPalleteArray[input.BlendIndices.w]);

    output.PosWorld = mul(float4(input.PosVertex, 1), matWorldBlended);
    output.PosProj = mul(float4(output.PosWorld, 1.f), View);
    output.PosProj = mul(output.PosProj, Projection);
    output.NorWorld = normalize(mul(input.NorVertex, (float3x3) matWorldBlended));
    output.TanWorld = normalize(mul(input.TanVertex, (float3x3) matWorldBlended));
    output.BitWorld = normalize(mul(input.BitVertex, (float3x3) matWorldBlended));
    output.TexCoord = input.TexCoord;

    return output;
}