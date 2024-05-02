#pragma once
#include "Component.h"
#include <d3d11.h>
#include <directxtk/simplemath.h>

class TransformComponent :
    public Component
{
public:
    TransformComponent(std::weak_ptr<Actor> owner);
    virtual ~TransformComponent();

public:
    virtual void Update(float deltaTime) override;

public:
    void CalculateRotationQuaternion();
    void CalculateTransformMatrix();

public:
    void SetPosition(const DirectX::SimpleMath::Vector3& position);
    void SetRotation(const DirectX::SimpleMath::Vector3& rotation);
    void SetScale(const DirectX::SimpleMath::Vector3& scale);

    DirectX::SimpleMath::Vector3 GetPosition() const;
    DirectX::SimpleMath::Vector3 GetRotation() const;
    DirectX::SimpleMath::Vector3 GetScale() const;

    DirectX::SimpleMath::Matrix GetTransformMatrix() const;

private:
    DirectX::SimpleMath::Vector3 m_Position;
    DirectX::SimpleMath::Vector3 m_Rotation;
    DirectX::SimpleMath::Vector3 m_Scale;

    DirectX::SimpleMath::Quaternion m_RotationQuaternion;

    DirectX::SimpleMath::Matrix m_TransformMatrix;
};

