#include "pch.h"
#include "Actor.h"
#include "../D3D/ModelLoader.h"
#include "Component.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "AnimationComponent.h"

int Actor::actorCount = 0;

Actor::Actor()
{
	m_ActorName = L"Actor" + std::to_wstring(actorCount);
	actorCount++;
}

Actor::~Actor()
{
}

void Actor::Update(float deltaTime)
{
	for (auto& co : m_Components)
	{
		co->Update(deltaTime);
	}
}

void Actor::Render(ID3D11DeviceContext* deviceContext)
{
	for (auto& co : m_Components)
	{
		co->Render(deviceContext);
	}
}

void Actor::Load(std::string filename)
{
	ModelLoader::GetInstance()->Load(filename, this);
}

std::wstring Actor::GetActorName()
{
	return m_ActorName;
}
