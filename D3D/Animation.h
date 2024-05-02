#pragma once
#include <vector>
#include <string>
#include <memory>
#include <directxtk/simplemath.h>
#include "NodeAnimation.h"


using namespace DirectX::SimpleMath;

class Animation
{
public:
	void Update(float dt);
	void AddNodeAnimation(std::shared_ptr<NodeAnimation> nodeAnim) { m_NodeAnimations.push_back(nodeAnim); }

public:
	void SetName(std::string name) { m_Name = name; }
	void SetAnimationDuration(float duration) { m_AnimationDuration = duration; }
	void SetAnimationSpeed(float speed) { m_AnimationSpeed = speed; }
	void SetAnimationProgress(float Progress) { m_AnimationProgress = Progress; }
	void SetIsLoop(bool loop) { m_IsLoop = loop; }

	std::string& GetName() { return m_Name; }
	std::vector<std::shared_ptr<NodeAnimation>>& GetNodeAnimations() { return m_NodeAnimations; }
	float& GetAnimationDuration() { return m_AnimationDuration; }
	float& GetAnimationSpeed() { return m_AnimationSpeed; }
	float& GetAnimationProgress() { return m_AnimationProgress; }

	Animation() = default;
	Animation(const Animation& other)
	{
		m_NodeAnimations = other.m_NodeAnimations;
		m_AnimationProgress = other.m_AnimationProgress;
		m_AnimationSpeed = other.m_AnimationSpeed;
		m_AnimationDuration = other.m_AnimationDuration;
		m_IsLoop = other.m_IsLoop;
		m_Name = other.m_Name;
	}
	~Animation();

private:
	std::vector<std::shared_ptr<NodeAnimation>> m_NodeAnimations;

	float m_AnimationDuration;
	float m_AnimationSpeed = 1.0f;
	float m_AnimationProgress = 0.0f;

	bool m_IsLoop;

	std::string m_Name;
};

