#pragma once
#include <directxtk/simplemath.h>
#include <string>
#include <memory>
#include "../Engine/Helper.h"

using namespace DirectX::SimpleMath;
using namespace DirectX;

class Node;

struct VERTEX
{
	Vector3 Position;	// 위치 정보.
	Vector2 TextureCoord;
	Vector3 Normal;
	Vector3 Tangent;
	Vector3 BiTangent;

	int BlendIndecses[4] = {0, };
	float BlendWeights[4] = {0, };

	VERTEX() {};
	VERTEX(Vector3 position, Vector2 uv, Vector3 normal, Vector3 tangent)
		: Position(position), TextureCoord(uv), Normal(normal), Tangent(tangent) {}

	void AddBoneData(int boneIndex, float weight)
	{
		assert(BlendWeights[0] == 0.f || BlendWeights[1] == 0.f || BlendWeights[2] == 0.f || BlendWeights[3] == 0.f);
		for (int i = 0; i < 4; i++)
		{
			if (BlendWeights[i] == 0.0f)
			{
				BlendIndecses[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};

struct BoneInfo
{
	Matrix BoneOffset;
	std::string BoneName;
	std::shared_ptr<Node> Onwer;

};

struct CB_Light
{
	DirectX::SimpleMath::Vector4 lightDir;
	DirectX::SimpleMath::Vector4 lightDiffuse;
	DirectX::SimpleMath::Vector4 lightAmbient;
	DirectX::SimpleMath::Vector4 lightSpecular;
	DirectX::SimpleMath::Vector3 viewPosition;
	float pad;
};


struct CB_MatrixPallete
{
	Matrix Array[128];
};