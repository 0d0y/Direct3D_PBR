#include <cassert>

#include "NodeAnimation.h"

Matrix NodeAnimation::Evaluate(float time) const
{
	// 보간할 키프레임 수 확인
	if (m_AnimationKeys.empty())
	{
		return Matrix::Identity;
	}

	if (m_AnimationKeys.size() == 1)
	{
		const Keyframe& keyframe = m_AnimationKeys[0];
		return Matrix::CreateScale(keyframe.scale) * Matrix::CreateFromQuaternion(keyframe.rotation) * Matrix::CreateTranslation(keyframe.position);
	}

	// 보간할 두 개의 키프레임 찾기
	size_t startIndex = 0;
	size_t endIndex = 1;

	for (size_t i = 0; i < m_AnimationKeys.size(); ++i)
	{
		if (time < m_AnimationKeys[i].time)
		{
			endIndex = i;
			break;
		}
		startIndex = i;
	}

	assert(endIndex - startIndex == 1);

	const Keyframe& startKeyframe = m_AnimationKeys[startIndex];
	const Keyframe& endKeyframe = m_AnimationKeys[endIndex];

	// 보간
	float t = (time - startKeyframe.time) / (endKeyframe.time - startKeyframe.time);
	Vector3 position = Vector3::Lerp(startKeyframe.position, endKeyframe.position, t);
	Quaternion rotation = Quaternion::Slerp(startKeyframe.rotation, endKeyframe.rotation, t);
	Vector3 scale = Vector3::Lerp(startKeyframe.scale, endKeyframe.scale, t);

	//Vector3 position = endKeyframe.position;
	//Quaternion rotation = endKeyframe.rotation;
	//Vector3 scale = endKeyframe.scale;

	// 변환 행렬 만들기
	Matrix localTransform = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);

	return localTransform;
}