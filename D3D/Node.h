#pragma once
#include <vector>
#include <string>
#include "../D3D/D3D.h"
#include <directxtk/simplemath.h>

using namespace DirectX::SimpleMath;

struct CB_Model
{
	DirectX::SimpleMath::Matrix WorldTransformMatrix;
};

class NodeAnimation;
class Animation;
class Model;
class Node
{
public:
	void Update(float dt);
	void Render(ID3D11DeviceContext* deviceContext, CB_MatrixPallete matrixPallete);

public:
	void SetName(std::string name) { m_Name = name; }
	void SetLocalTransform(Matrix tm) { m_LocalTransform = tm; }
	void SetWorldTransform(Matrix tm) { m_WorldTransform = tm; }
	void SetParentNode(std::shared_ptr<Node> parentNode) { m_Parent = parentNode; }
	void SetNodeAnimation(std::shared_ptr<NodeAnimation> nodeAnim) { m_NodeAnimation = nodeAnim; }
	void SetSelectedAnimation(std::shared_ptr<Animation> Anim) { m_SelectedAnimation = Anim; }

	std::vector<Mesh>& GetMeshes() { return m_Meshes; }
	std::vector<std::shared_ptr<Node>>& GetNodes() { return m_Children; }
	Matrix& GetLocalTransformMatrix() { return m_LocalTransform; }
	Matrix& GetWorldTransformMatrix() { return m_WorldTransform; }
	std::string& GetName() { return m_Name; }

public:
	Node() = default;
	Node(const Node& other)
	{
		m_Name = other.m_Name;

		m_Parent = other.m_Parent;
		m_Meshes = other.m_Meshes;

		m_Children = other.m_Children;

		m_SelectedAnimation = other.m_SelectedAnimation;
		m_NodeAnimation = other.m_NodeAnimation;

		m_LocalTransform = other.m_LocalTransform;
		m_WorldTransform = other.m_WorldTransform;
	}
	~Node();

	void UpdateMatrix();

	//debug
	void PrintNodes();

private:
	std::string m_Name;

	std::shared_ptr<Node> m_Parent;
	std::vector<Mesh> m_Meshes;

	std::vector<std::shared_ptr<Node>> m_Children;

	std::shared_ptr<Animation> m_SelectedAnimation;
	std::shared_ptr<NodeAnimation> m_NodeAnimation;

	Matrix m_LocalTransform;
	Matrix m_WorldTransform;
};