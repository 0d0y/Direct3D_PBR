#include "pch.h"
#include "../D3D/Animation.h"
#include "../D3D/Node.h"
#include "Actor.h"
#include "AnimationComponent.h"
#include "RenderComponent.h"

AnimationComponent::AnimationComponent(std::weak_ptr<Actor> onwer)
	: Component(onwer, typeid(this).name())
	, m_PlayingAnimation(nullptr)
{
}

AnimationComponent::~AnimationComponent()
{
}

void AnimationComponent::Update(float deltaTime)
{
	m_PlayingAnimation->Update(deltaTime);
}

void AnimationComponent::AddAnimation(std::shared_ptr<Animation> anim)
{
	m_Animations.push_back(anim);
}

void AnimationComponent::SelectAnimation(const std::string& animationName)
{
	std::shared_ptr<Animation> selectedAnimation;
	for (auto& anim : m_Animations)
	{
		if (anim->GetName() == animationName)
		{
			selectedAnimation = anim;
			break;
		}
	}

	if (selectedAnimation == nullptr)
		return;

	m_PlayingAnimation = selectedAnimation;

	auto rootNode = m_Owner.lock()->GetComponent<RenderComponent>()->GetRootNode();

	SetNodeAnimationsFromAnimation(selectedAnimation, rootNode);
}

void AnimationComponent::SetNodeAnimationsFromAnimation(std::shared_ptr<Animation> animation, std::shared_ptr<Node> node)
{
	node->SetSelectedAnimation(animation);

	for (auto& nodeAnim : animation->GetNodeAnimations())
	{
		if (node->GetName() == nodeAnim->GetName())
			node->SetNodeAnimation(nodeAnim);
	}

	for (auto& child : node->GetNodes())
	{
		SetNodeAnimationsFromAnimation(animation, child);
	}
}

void AnimationComponent::SetIsAnimationLoop(bool loop)
{
	if (m_PlayingAnimation != nullptr)
		m_PlayingAnimation->SetIsLoop(loop);
}

void AnimationComponent::SetAnimationSpeed(float speed)
{
	if (m_PlayingAnimation != nullptr)
		m_PlayingAnimation->SetAnimationSpeed(speed);
}

float& AnimationComponent::GetAnimationSpeed()
{
	if (m_PlayingAnimation != nullptr)
		return m_PlayingAnimation->GetAnimationSpeed();
}
