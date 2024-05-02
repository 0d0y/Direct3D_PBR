#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <directxtk/simplemath.h>
#include <assimp/scene.h>
#include "Materials.h"

class Node;

class Mesh
{
private:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;

	std::vector<VERTEX> m_Vertices;
	std::vector<UINT> m_Indices;

	Materials m_Materials;

	std::vector<BoneInfo> m_BoneReferences;

public:
	Mesh(ID3D11Device* device, const std::vector<VERTEX>& vertices, const std::vector<UINT>& indices, std::vector<BoneInfo>& boneReferences) :
		m_VertexBuffer(nullptr),
		m_IndexBuffer(nullptr),
		m_Vertices(vertices),
		m_Indices(indices),
		m_BoneReferences(boneReferences)
	{
		this->setupMesh(device);
	}
private:
	void setupMesh(ID3D11Device* device);

public:
	void SetupPipeline(ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11DeviceContext* deviceContext);
	void Close();

	void UpdateMatrixPallete(Matrix* matrixPalletePtr);

public:
	ID3D11Buffer* GetVertexBuffer() { return m_VertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_IndexBuffer; }

	Materials& GetMaterials() { return m_Materials; }
	std::vector<BoneInfo>& GetBoneReferences() { return m_BoneReferences; }

};
