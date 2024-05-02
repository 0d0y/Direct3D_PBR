#include "Animation.h"
#include "../Engine/Helper.h"

void Animation::Update(float dt)
{
	m_AnimationProgress += dt * m_AnimationSpeed;

	if (m_AnimationProgress > m_AnimationDuration)
	{
		if (m_IsLoop)
			m_AnimationProgress = fmod(m_AnimationProgress, m_AnimationDuration);
		else
			m_AnimationProgress = min(m_AnimationProgress, m_AnimationDuration);
	}
}

Animation::~Animation()
{
	//for (auto& nodeAnim : m_NodeAnimations)
	//{
	//	SAFE_DELETE(nodeAnim);
	//}
	//m_NodeAnimations.clear();
}
