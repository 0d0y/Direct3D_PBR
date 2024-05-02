#include "Mesh.h"
#include "Node.h"
#include <stdexcept>

void Mesh::setupMesh(ID3D11Device* device)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(VERTEX) * m_Vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_Vertices[0];

	hr = device->CreateBuffer(&vbd, &initData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		Close();
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &m_Indices[0];

	hr = device->CreateBuffer(&ibd, &initData, &m_IndexBuffer);
	if (FAILED(hr))
	{
		Close();
		throw std::runtime_error("Failed to create index buffer.");
	}
}

void Mesh::SetupPipeline(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_Materials.SetupPipeline(deviceContext);
}

void Mesh::Draw(ID3D11DeviceContext* deviceContext)
{
	deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void Mesh::Close()
{
	SAFE_RELEASE(m_VertexBuffer);
	SAFE_RELEASE(m_IndexBuffer);
}

void Mesh::UpdateMatrixPallete(Matrix* matrixPalletePtr)
{
	assert(m_BoneReferences.size() < 128);
	for (UINT i = 0; i < m_BoneReferences.size(); i++)
	{
		Matrix& BoneNodeWorldMatrix = m_BoneReferences[i].Onwer->GetWorldTransformMatrix();
		matrixPalletePtr[i] = m_BoneReferences[i].BoneOffset * BoneNodeWorldMatrix;
		matrixPalletePtr[i] = matrixPalletePtr[i].Transpose();
	}
}
