#include "Materials.h"
#include "D3D.h"

void Materials::SetHasMap(aiTextureType type, int hasmap)
{
	m_HasMap[type] = hasmap;
}

int Materials::GetShaderRegisterForType(aiTextureType type)
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		return 0;
		break;
	case aiTextureType_NORMALS:
		return 1;
		break;
	case aiTextureType_SPECULAR:
		return 2;
		break;
	case aiTextureType_OPACITY:
		return 3;
		break;
	case aiTextureType_EMISSIVE:
		return 4;
		break;
	case aiTextureType_SHININESS:
		return 5;
		break;
	case aiTextureType_METALNESS:
		return 6;
		break;
	default:
		return -1;
		break;
	}
}

void Materials::SetupPipeline(ID3D11DeviceContext* deviceContext)
{
	deviceContext->PSSetConstantBuffers(3, 1, &m_HasMapBuffer);

	for (auto& loadedTexture : m_Material)
	{
		int registerSlot = GetShaderRegisterForType(loadedTexture.GetType());
		deviceContext->PSSetShaderResources(registerSlot, 1, &loadedTexture.GetTexture());
	}
}

void Materials::Close()
{
	SAFE_RELEASE(m_HasMapBuffer);
}

void Materials::UpdateHasMapConstantBuffer(ID3D11DeviceContext* deviceContext)
{
	HasMapFlag HasMapData;
	HasMapData.HasDiffuseMap = m_HasMap[aiTextureType::aiTextureType_DIFFUSE];
	HasMapData.HasEmissiveMap = m_HasMap[aiTextureType::aiTextureType_EMISSIVE];
	HasMapData.HasNormalMap = m_HasMap[aiTextureType::aiTextureType_NORMALS];
	HasMapData.HasOpacityMap = m_HasMap[aiTextureType::aiTextureType_OPACITY];
	HasMapData.HasSpecularMap = m_HasMap[aiTextureType::aiTextureType_SPECULAR];
	HasMapData.HasRoughNessMap = m_HasMap[aiTextureType::aiTextureType_SHININESS];
	HasMapData.HasMetalnessMap = m_HasMap[aiTextureType::aiTextureType_METALNESS];

	deviceContext->UpdateSubresource(m_HasMapBuffer, 0, nullptr, &HasMapData, 0, 0);
}

Materials::Materials()
{
	m_HasMapBuffer = D3D::GetInstance()->CreateConstantBuffer(sizeof(HasMapFlag));
}
