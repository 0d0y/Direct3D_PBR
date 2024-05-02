#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <directxtk/simplemath.h>

using namespace DirectX::SimpleMath;


struct Keyframe
{
    float time;
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
};

class NodeAnimation
{
public:
    Matrix Evaluate(float time) const;
    void AddKeyframe(Keyframe keyframe) { m_AnimationKeys.push_back(keyframe); }

public:
    std::string& GetName() { return m_NodeName; }
    std::vector<Keyframe>& GetAnimationKeys() { return m_AnimationKeys; }

    void SetNodeName(std::string name) { m_NodeName = name; }

private:
    std::string m_NodeName;
    std::vector<Keyframe> m_AnimationKeys;

};

