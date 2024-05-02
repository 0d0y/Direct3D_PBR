#include "Node.h"
#include "NodeAnimation.h"
#include "Animation.h"
#include <iostream>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

void Node::Update(float dt)
{
	this->UpdateMatrix();

	for (auto& node : m_Children)
	{
		node->Update(dt);
	}
}

void Node::Render(ID3D11DeviceContext* deviceContext, CB_MatrixPallete matrixPallete)
{
	//PrintNodes();
	D3D::GetInstance()->UpdateTransformMatrixConstantBuffer(m_WorldTransform.Transpose());

	for (auto& mesh : m_Meshes)
	{
		mesh.UpdateMatrixPallete(matrixPallete.Array);
		D3D::GetInstance()->UpdateMatrixPalleteConstantBuffer(matrixPallete);
		mesh.SetupPipeline(deviceContext);
		mesh.Draw(deviceContext);
	}
	for (auto& node : m_Children)
	{
		node->Render(deviceContext, matrixPallete);
	}
}

Node::~Node()
{
	//for (auto& node : m_Children)
	//{
	//	SAFE_DELETE(node);
	//}
	m_Children.clear();
}



void Node::UpdateMatrix()
{
	if (m_NodeAnimation)
	{
		Matrix animationTransform = m_NodeAnimation->Evaluate(m_SelectedAnimation->GetAnimationProgress());
		m_LocalTransform = animationTransform;
	}

	if (m_Parent != nullptr)
	{
		m_WorldTransform = m_LocalTransform * m_Parent->GetWorldTransformMatrix();
	}

	else
	{
		m_WorldTransform = m_LocalTransform;
	}
}

void Node::PrintNodes()
{
	std::cout << "My Name : " << m_Name << std::endl;

	auto parent = m_Parent;
	if (parent)
		std::cout << "Parent Name :" << parent->GetName() << std::endl;
	std::cout << "Children Name : ";
	for (auto& node : m_Children)
	{
		std::cout << node->GetName() << ", ";
	}
	std::cout << std::endl;
}