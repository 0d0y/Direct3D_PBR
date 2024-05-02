#pragma once
#include "struct.h"
#include "Material.h"

struct HasMapFlag
{
	int HasDiffuseMap;
	int HasNormalMap;
	int HasSpecularMap;
	int HasOpacityMap;
	int HasEmissiveMap;
	int HasRoughNessMap;
	int HasMetalnessMap;
	int pad[1];
};

class Materials
{
public:
	void SetHasMap(aiTextureType type, int hasmap);
	void SetupPipeline(ID3D11DeviceContext* deviceContext);
	void Close();

	int GetShaderRegisterForType(aiTextureType type);
	void UpdateHasMapConstantBuffer(ID3D11DeviceContext* deviceContext);

public:
	std::vector<Material>& GetMaterial() { return m_Material; }

public:
	Materials();

private:
	ID3D11Buffer* m_HasMapBuffer;
	std::vector<Material> m_Material;

	bool m_HasMap[aiTextureType::aiTextureType_UNKNOWN] = { false, };
};

