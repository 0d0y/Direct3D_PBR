#pragma once
#include "Component.h"

class Animation;
class Node;

class AnimationComponent
	: public Component
{
public:
	AnimationComponent(std::weak_ptr<Actor> onwer);
	virtual ~AnimationComponent();

public:
	virtual void Update(float deltaTime) override;

public:
	void AddAnimation(std::shared_ptr<Animation> anim);
	void SelectAnimation(const std::string& animationName);
	void SetNodeAnimationsFromAnimation(std::shared_ptr<Animation> animation, std::shared_ptr<Node> node);

public:
	void SetIsAnimationLoop(bool loop);
	void SetAnimationSpeed(float speed);
	float& GetAnimationSpeed();

private:
	std::vector<std::shared_ptr<Animation>> m_Animations;
	std::shared_ptr<Animation> m_PlayingAnimation;
};

