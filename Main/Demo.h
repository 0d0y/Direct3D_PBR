#pragma once
#include "../Engine/Yngine.h"
#include <vector>
#include "../D3D/D3D.h"
#include <memory>

class Camera;
class ModelLoader;
class Model;
class Actor;

class Demo :
	public Yngine
{
private:
	Camera* m_Camera;


	//Model* m_Tree;
	//Model* m_Soldier;
	//Model* m_Walker;
	//Model* m_Dancer1;
	//Model* m_Dancer2;
	//Model* m_Dancer3;
	std::shared_ptr<Actor> m_Zelda;
	std::shared_ptr<Actor> m_Dancer4;
	std::shared_ptr<Actor> m_Gun;
	//temp
	DirectX::SimpleMath::Vector3 m_ActorTranslate;
	DirectX::SimpleMath::Vector3 m_ActorRotation;

	CB_Light m_DirectLight;

public:
	Demo();
	~Demo() override;

protected:
	void UpdateAll() override;
	void RenderAll() override;


	void InitImGui();
	void CleanupImGui();
	void RenderImGui();

public:
	/*Init*/
	HRESULT Initialize(HINSTANCE hInst) override;
	/*unInit*/
	void Finalize() override;

	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
};

