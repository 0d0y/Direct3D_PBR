#include "pch.h"
#include "Component.h"
#include "Actor.h"

std::unordered_map<std::wstring, std::unordered_map<std::wstring, unsigned int>> Component::componentCountersPerActor;

Component::Component(std::weak_ptr<Actor> owner, const char* typeName)
	: m_Owner(owner)
{
	std::wstring componentName = std::wstring(typeName, typeName + std::strlen(typeName));
	m_ComponentID = componentCountersPerActor[m_Owner.lock()->GetActorName()][componentName]++;
	m_ComponentName = m_Owner.lock()->GetActorName() + L"_" + componentName + L"_" + std::to_wstring(m_ComponentID);
}


Component::~Component()
{
}

void Component::Render(ID3D11DeviceContext* deviceContext)
{
}

std::weak_ptr<Actor> Component::GetOwner()
{
	return m_Owner;
}