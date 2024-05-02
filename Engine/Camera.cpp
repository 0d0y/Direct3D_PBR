#include "pch.h"
#include "Camera.h"
#include <DirectXMath.h>

using namespace DirectX;

Camera::Camera()
	: m_Position(0.0f, 0.0f, 0.0f)
	, m_Yaw(0.0f)
	, m_Pitch(0.0f)
	, m_Roll(0.0f)
	, m_FOV(45.f)
	, m_NearPlane(1.f)
	, m_FarPlane(10000.0f)
	, m_AspectRatio(1.f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(const Vector3& position)
{
	m_Position = position;
}

void Camera::SetRotation(float yaw, float pitch, float roll)
{
	m_Yaw = yaw;
	m_Pitch = pitch;
	m_Roll = roll;
}

Matrix Camera::GetViewMatrix() const
{
	Matrix rotationMatrix = XMMatrixRotationRollPitchYaw(m_Pitch, m_Yaw, m_Roll);
	Matrix translationMatrix = XMMatrixTranslation(-m_Position.x, -m_Position.y, -m_Position.z);
	return rotationMatrix * translationMatrix;
}

Matrix Camera::GetProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
}
