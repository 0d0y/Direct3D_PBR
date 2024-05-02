#include "pch.h"
#include "TransformComponent.h"
#include <directxtk/simplemath.h>

using namespace DirectX::SimpleMath;

TransformComponent::TransformComponent(std::weak_ptr<Actor> owner)
	: Component(owner, typeid(this).name())
	, m_Position(Vector3::Zero)
	, m_Rotation(Vector3::Zero)
	, m_Scale(Vector3::One)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::Update(float deltaTime)
{
	CalculateRotationQuaternion();
	CalculateTransformMatrix();
}

void TransformComponent::CalculateRotationQuaternion()
{
	m_RotationQuaternion = Quaternion::CreateFromYawPitchRoll(m_Rotation.y * DirectX::XM_PI / 180.0f, m_Rotation.x * DirectX::XM_PI / 180.0f, m_Rotation.z * DirectX::XM_PI / 180.0f);
}

void TransformComponent::CalculateTransformMatrix()
{
	m_TransformMatrix = Matrix::CreateScale(m_Scale) * Matrix::CreateFromQuaternion(m_RotationQuaternion) * Matrix::CreateTranslation(m_Position);
}

void TransformComponent::SetPosition(const Vector3& position)
{
	m_Position = position;
}

void TransformComponent::SetRotation(const Vector3& rotation)
{
	m_Rotation = rotation;
}

void TransformComponent::SetScale(const Vector3& scale)
{
	m_Scale = scale;
}

Vector3 TransformComponent::GetPosition() const
{
	return m_Position;
}

Vector3 TransformComponent::GetRotation() const
{
	return m_Rotation;
}

Vector3 TransformComponent::GetScale() const
{
	return 	m_Scale;
}

DirectX::SimpleMath::Matrix TransformComponent::GetTransformMatrix() const
{
	return m_TransformMatrix;
}
