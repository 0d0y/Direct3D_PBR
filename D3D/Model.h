#pragma once
#include <string>
#include <vector>
#include "../D3D/D3D.h"
#include "struct.h"
#include "Materials.h"
#include "Animation.h"

class Node;

class Model
{
public:
	void Load(std::string filename);
	void Update(float dt);
	void Draw(ID3D11DeviceContext* deviceContext);

	void Close();

	void AddAnimation(std::shared_ptr<Animation> anim) { m_Animations.push_back(anim); }
	void SelectAnimation(const std::string& animationName);
	void SetNodeAnimationsFromAnimation(std::shared_ptr<Animation> animation, std::shared_ptr<Node> node);

public:
	std::wstring& GetVectexShaderKey() { return m_VectexShaderKey; }
	std::wstring& GetPixelShaderKey() { return m_PixelShaderKey; }

	DirectX::SimpleMath::Vector3& GetPosition() { return m_Position; }
	DirectX::SimpleMath::Quaternion& GetRotation() { return m_Rotation; }
	DirectX::SimpleMath::Vector3& GetScale() { return m_Scale; }

	//temp
	float& GetSpecularPower() { return m_SpecularPower; }
	DirectX::SimpleMath::Vector3& GetAngle() { return m_Angle; }
	void SetAngle(Vector3 Angle) { m_Angle = Angle; }

	std::shared_ptr<Node> GetRootNode() { return m_RootNode; }
	//Matrix& GetTransformMatrix() { return m_Transform; }

	float& GetAnimationSpeed() { if (m_PlayingAnimation) return m_PlayingAnimation->GetAnimationSpeed(); }


public:
	void SetPosition(DirectX::SimpleMath::Vector3 pos) { m_Position = pos; }
	void SetRotation(Quaternion rota) { m_Rotation = rota; }
	void SetScale(DirectX::SimpleMath::Vector3 scal) { m_Scale = scal; }

	void SetVectexShaderKey(std::wstring_view vectexShaderKey) { m_VectexShaderKey = vectexShaderKey; }
	void SetPixelShaderKey(std::wstring_view pixelShaderKey) { m_PixelShaderKey = pixelShaderKey; }

	void SetRootNode(std::shared_ptr<Node> rootNode) { m_RootNode = rootNode; }

	void SetIsAnimationLoop(bool loop) { if (m_PlayingAnimation) m_PlayingAnimation->SetIsLoop(loop); }
	void SetAnimationSpeed(float speed) { if (m_PlayingAnimation) m_PlayingAnimation->SetAnimationSpeed(speed); }

public:
	Model();
	~Model();


private:

	std::vector<std::shared_ptr<Animation>> m_Animations;
	std::shared_ptr<Animation> m_PlayingAnimation;

	DirectX::SimpleMath::Vector3 m_Position;
	DirectX::SimpleMath::Quaternion m_Rotation;
	DirectX::SimpleMath::Vector3 m_Scale;
	DirectX::SimpleMath::Vector3 m_Angle = {0.f, 0.f, 0.f};

	std::shared_ptr<Node> m_RootNode;
	std::wstring m_VectexShaderKey;
	std::wstring m_PixelShaderKey;
	CB_MatrixPallete m_MatrixPallete;

	//temp
	float m_SpecularPower = 2000.f;

private:
	void UpdateMatrix();


	//debug
	void PrintNodeNames(Node* node, int depth);
	void PrintNodeNames();
};

