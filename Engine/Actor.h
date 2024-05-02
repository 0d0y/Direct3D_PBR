#pragma once
#include "pch.h"

class Component;
struct ID3D11DeviceContext;

class Actor : public std::enable_shared_from_this<Actor>
{
public:
	Actor();
	~Actor();

public:
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* deviceContext);

public:
	void Load(std::string filename);

public:
	std::wstring GetActorName();

	template<typename T>
	std::weak_ptr<T> AddComponent();
	template<typename T>
	std::shared_ptr<T> GetComponent();

private:
	std::vector<std::shared_ptr<Component>> m_Components;
	std::wstring m_ActorName;

	static int actorCount;
};

template<typename T>
inline std::weak_ptr<T> Actor::AddComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T는 컴포넌트 클래스의 파생 클래스여야 함.");

	std::shared_ptr<T> component = std::make_shared<T>(shared_from_this());
	m_Components.push_back(component);

	return component;
}

template<typename T>
inline std::shared_ptr<T> Actor::GetComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T는 컴포넌트 클래스의 파생 클래스여야 함.");

	for (const auto& component : m_Components)
	{
		std::shared_ptr<T> typedComponent = std::dynamic_pointer_cast<T>(component);

		if (typedComponent)
		{
			return typedComponent;
		}
	}

	return std::shared_ptr<T>();
}
