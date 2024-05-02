#pragma once

#include <string>
#include "struct.h"
#include <assimp\material.h>

class Material
{
public:
	Material()
		: m_Texture(nullptr)
		, m_Path()
		, m_Type()
	{

	}

	Material(ID3D11ShaderResourceView* texture, std::string path, aiTextureType type)
		: m_Texture(texture)
		, m_Path(path)
		, m_Type(type)
	{

	}

	void Release()
	{
		SAFE_RELEASE(m_Texture);
	}


public:
	ID3D11ShaderResourceView*& GetTexture() { return m_Texture; }
	std::string& GetPath() { return m_Path; }
	aiTextureType& GetType() { return m_Type; }

private:
	ID3D11ShaderResourceView* m_Texture;
	std::string m_Path;
	aiTextureType m_Type;
};