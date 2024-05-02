#pragma once
#include <unordered_map>

class Actor;
struct ID3D11DeviceContext;

class Component
{
public:
	Component(std::weak_ptr<Actor> owner, const char* typeName);
	virtual ~Component();

public:
	virtual void Update(float DeltaTime) = 0;
	virtual void Render(ID3D11DeviceContext* deviceContext);

public:
	std::weak_ptr<Actor> GetOwner();

protected:
	std::weak_ptr<Actor> m_Owner;

	std::wstring m_ComponentName;
	unsigned int m_ComponentID;

private:
	static std::unordered_map<std::wstring, std::unordered_map<std::wstring, unsigned int>> componentCountersPerActor;
};

