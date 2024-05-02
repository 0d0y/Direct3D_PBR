#include "pch.h"
#include "RenderComponent.h"
#include "../D3D/Node.h"
#include "Actor.h"
#include "TransformComponent.h"

RenderComponent::RenderComponent(std::weak_ptr<Actor> owner)
    : Component(owner, typeid(this).name())
    , m_RootNode(nullptr)
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::Update(float deltaTime)
{
    m_RootNode->SetLocalTransform(m_Owner.lock()->GetComponent<TransformComponent>()->GetTransformMatrix());
    m_RootNode->Update(deltaTime);
}

void RenderComponent::Render(ID3D11DeviceContext* deviceContext)
{
    m_RootNode->Render(deviceContext, m_MatrixPallete);
}

std::wstring& RenderComponent::GetVertexShaderKey()
{
    return m_VertexShaderKey;
}

std::wstring& RenderComponent::GetPixelShaderKey()
{
    return m_PixelShaderKey;
}

void RenderComponent::SetVertexShaderKey(std::wstring_view vertexShaderKey)
{
    m_VertexShaderKey = vertexShaderKey;
}

void RenderComponent::SetPixelShaderKey(std::wstring_view pixelShaderKey)
{
    m_PixelShaderKey = pixelShaderKey;
}

std::shared_ptr<Node> RenderComponent::GetRootNode()
{
    return m_RootNode;
}

void RenderComponent::SetRootNode(std::shared_ptr<Node> rootNode)
{
    m_RootNode = rootNode;
}
