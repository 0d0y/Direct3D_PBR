#pragma once
#include "Component.h"
#include "../D3D/struct.h"

class Node;

class RenderComponent :
    public Component
{
public:
	RenderComponent(std::weak_ptr<Actor> onwer);
	virtual ~RenderComponent();

public:
	virtual void Update(float deltaTime) override;
	virtual void Render(ID3D11DeviceContext* deviceContext) override;

public:
	std::wstring& GetVertexShaderKey();
	std::wstring& GetPixelShaderKey();
	void SetVertexShaderKey(std::wstring_view vertexShaderKey);
	void SetPixelShaderKey(std::wstring_view pixelShaderKey);
	std::shared_ptr<Node> GetRootNode();

	void SetRootNode(std::shared_ptr<Node> rootNode);

private:
	std::shared_ptr<Node> m_RootNode;

	std::wstring m_VertexShaderKey;
	std::wstring m_PixelShaderKey;

	CB_MatrixPallete m_MatrixPallete;
};

