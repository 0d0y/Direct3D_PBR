//--------------------------------------------------------------------------------------
// File: Tutorial06.fx
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D txNormal : register(t1);
Texture2D txSpecular : register(t2);
Texture2D txOpacity : register(t3);
Texture2D txEmissive : register(t4);
Texture2D txRoughness : register(t5);
Texture2D txMetalness : register(t6);
SamplerState samLinear : register(s0);
BlendState blendState : register(s1);

cbuffer ViewProjection : register(b0)
{
    matrix View;
    matrix Projection;
}

cbuffer Model : register(b1)
{
    matrix World;
}

cbuffer DirectionLight : register(b2)
{
    float4 lightDir;
    float4 lightDiffuse;
    float4 lightAmbient;
    float4 lightSpecular;
    float3 viewPosition;
    float pad_0;
}

cbuffer HasMap : register(b3)
{
    int HasDiffuseMap;
    int HasNormalMap;
    int HasSpecularMap;
    int HasOpacityMap;
    int HasEmissiveMap;
    int HasRoughnessMap;
    int HasMetalnessMap;
}

cbuffer MatrixPallete : register(b4)
{
    matrix MatrixPalleteArray[128];
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 PosVertex : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 NorVertex : NORMAL;
    float3 TanVertex : TANGENT;
    float3 BitVertex : BINORMAL;
    int4 BlendIndices : BLENDINDICES;
    float4 BlendWeights : BLENDWEIGHT;
};

struct PS_INPUT
{
    float4 PosProj : SV_POSITION;
    float3 PosWorld : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 NorWorld : TEXCOORD1;
    float3 TanWorld : TEXCOORD2;
    float3 BitWorld : TEXCOORD4;
};