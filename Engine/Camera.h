#pragma once
#include <directxtk/simplemath.h>

using namespace DirectX::SimpleMath;
class Camera
{
public:
	Camera();
	~Camera();

	void SetPosition(const Vector3& position);
	void SetRotation(float yaw, float pitch, float roll);
	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix() const;
	Vector3& GetCameraPosition() { return m_Position; }

	void SetFOV(float fov) { m_FOV = fov; }
	void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
	void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }
	void SetAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; }

	float& GetFOV() { return m_FOV; }
	float& GetNearPlane() { return m_NearPlane; }
	float& GetFarPlane() { return m_FarPlane; }
	float& GetAspectRatio() { return m_AspectRatio; }


private:
	Vector3 m_Position;
	float m_Yaw;
	float m_Pitch;
	float m_Roll;

	float m_FOV;
	float m_NearPlane;
	float m_FarPlane;

	//화면 가로 세로 비율 (Aspect Ratio)
	float m_AspectRatio;
};