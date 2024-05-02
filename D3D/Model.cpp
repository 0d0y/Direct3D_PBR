#include "Model.h"
#include "../D3D/ModelLoader.h"
#include "../Engine/Helper.h"
#include "Node.h"
#include <iostream>

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

void Model::Load(std::string filename)
{
	//ModelLoader::GetInstance()->Load(filename, this);
}

void Model::Update(float dt)
{
	UpdateMatrix();
	if (m_PlayingAnimation)
		m_PlayingAnimation->Update(dt);
	if (m_RootNode)
	{
		m_RootNode->Update(dt);
	}
}

void Model::Draw(ID3D11DeviceContext* deviceContext)
{
	m_RootNode->Render(deviceContext, m_MatrixPallete);
	//PrintNodeNames();
}

void Model::Close()
{
	//SAFE_DELETE(m_RootNode);
	//for (auto& anim : m_Animations)
	//{
	//	SAFE_DELETE(anim);
	//}
	//m_Animations.clear();
}

void Model::SelectAnimation(const std::string& animationName)
{
	std::shared_ptr<Animation> selectedAnimation;
	for (auto& anim : m_Animations)
	{
		if (anim->GetName() == animationName)
		{
			selectedAnimation = anim;
			break;
		}
	}

	if (!selectedAnimation)
		return;

	m_PlayingAnimation = selectedAnimation;

	SetNodeAnimationsFromAnimation(selectedAnimation, m_RootNode);
}

void Model::SetNodeAnimationsFromAnimation(std::shared_ptr<Animation> animation, std::shared_ptr<Node> node)
{
	node->SetSelectedAnimation(animation);

	for (auto& nodeAnim : animation->GetNodeAnimations())
	{
		if (node->GetName() == nodeAnim->GetName())
			node->SetNodeAnimation(nodeAnim);
	}

	for (auto& child : node->GetNodes())
	{
		//SetNodeAnimationsFromAnimation(animation, child);
	}
}

Model::Model()
	: m_Position{ 0.f, 0.f, 0.f }
	, m_Rotation{ 0.f, 0.f, 0.f, 0.f }
	, m_Scale{ 1.f, 1.f, 1.f }
{
}

Model::~Model()
{
	this->Close();
}


void Model::UpdateMatrix()
{
	if (m_RootNode)
	{
		Matrix Model = Matrix::CreateScale(m_Scale) * Matrix::CreateFromQuaternion(m_Rotation) * Matrix::CreateTranslation(m_Position);
		m_RootNode->SetLocalTransform(Model);
	}
}



void Model::PrintNodeNames(Node* node, int depth = 0)
{
	for (int i = 0; i < depth; i++)
	{
		std::wcout << L"  ";
	}
	std::wcout << node->GetName().c_str() << std::endl;

	for (auto& child : node->GetNodes())
	{
		//PrintNodeNames(child, depth + 1);
	}
}

void Model::PrintNodeNames()
{
	if (m_RootNode)
	{
		//PrintNodeNames(m_RootNode);
	}
}