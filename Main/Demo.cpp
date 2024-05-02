#include "../Engine/Camera.h"
#include "../Engine/TimeSystem.h"
#include "Demo.h"

#include "../D3D/Model.h"
#include "../D3D/ModelLoader.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "../Engine/Actor.h"
#include "../Engine/Component.h"
#include "../Engine/TransformComponent.h"
#include "../Engine/RenderComponent.h"
#include "../Engine/AnimationComponent.h"


using namespace DirectX::SimpleMath;
using namespace DirectX;

Demo::Demo()
{
}

Demo::~Demo()
{
}

void Demo::UpdateAll()
{
	Yngine::UpdateAll();
	float dt = m_Timer->GetDeltaTime();

	//m_Zelda->Update(dt);
	//m_Dancer4->Update(dt);
	m_Gun->Update(dt);

	m_DirectLight.viewPosition = m_Camera->GetCameraPosition();
	m_D3DEngine->UpdateLightConstantBuffer(m_DirectLight);
	m_D3DEngine->UpdateViewProjConstantBuffer(XMMatrixTranspose(m_Camera->GetViewMatrix()), XMMatrixTranspose(m_Camera->GetProjectionMatrix()));
}


void Demo::RenderAll()
{
	Yngine::RenderAll();

	m_D3DEngine->PrepareRendering();

	//m_D3DEngine->Render(m_Zelda);
	//m_D3DEngine->Render(m_Dancer4);
	m_D3DEngine->Render(m_Gun);
	RenderImGui();

	m_D3DEngine->PresentFrame();
}

HRESULT Demo::Initialize(HINSTANCE hInst)
{
	Yngine::Initialize(hInst);
	InitImGui();
	ModelLoader::GetInstance()->Initialize(m_D3DEngine->GetDevice(), m_D3DEngine->GetDeviceContext(), m_hWnd);
	m_Camera = new Camera;

	m_D3DEngine->InitPipeline();
	m_D3DEngine->LoadVertexShader(L"../Shaders/StaticVertexShader.hlsl");
	m_D3DEngine->LoadVertexShader(L"../Shaders/SkeletalVertexShader.hlsl");
	m_D3DEngine->LoadPixelShader(L"../Shaders/PixelShader.hlsl");
	m_D3DEngine->LoadPixelShader(L"../Shaders/PBRPixelShader.hlsl");

	float aspectRatio = static_cast<float>(m_ResolutionWidth) / static_cast<float>(m_ResolutionHeight);
	m_Camera->SetAspectRatio(aspectRatio);
	m_Camera->SetPosition(Vector3(0.f, 0.f, -500.0f)); // 원하는 초기 위치로 설정
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f); // 원하는 초기 회전 각도로 설정

	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_D3DEngine->SetViewMatrix(XMMatrixLookAtLH(Eye, At, Up));
	m_D3DEngine->SetProjectionMatrix(XMMatrixPerspectiveFovLH(XM_PIDIV2, m_ResolutionWidth / (FLOAT)m_ResolutionHeight, 0.01f, 100.0f));

	m_DirectLight.lightDir = { 0.0f, 0.0f, 1.0f, 0.0f };
	m_DirectLight.lightDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_DirectLight.lightAmbient = { 94.f / 255.f, 94.f / 255.f, 94.f / 255.f, 1.0f };
	m_DirectLight.lightSpecular = { 1.0f, 1.0f, 1.0f, 1.0f };




	m_Zelda = std::make_unique<Actor>();
	m_Dancer4 = std::make_unique<Actor>();
	m_Gun = std::make_unique<Actor>();

	m_Gun->AddComponent<TransformComponent>();
	m_Gun->AddComponent<RenderComponent>();
	m_Gun->Load("../FBX/cerberus.fbx");
	auto gunTransformComponent = m_Gun->GetComponent<TransformComponent>();
	gunTransformComponent->SetPosition({ 0.f, 0.f, 0.f });
	gunTransformComponent->SetRotation({ 0.f, 0.f, 0.f });
	gunTransformComponent->SetScale({ 2.5f, 2.5f, 2.5f });
	auto gunRenderComponent = m_Gun->GetComponent<RenderComponent>();
	gunRenderComponent->SetVertexShaderKey(L"../Shaders/StaticVertexShader.hlsl");
	gunRenderComponent->SetPixelShaderKey(L"../Shaders/PBRPixelShader.hlsl");

	m_Zelda->AddComponent<TransformComponent>();
	m_Zelda->AddComponent<RenderComponent>();
	m_Zelda->Load("../FBX/zeldaPosed001.fbx");
	auto zeldaTransformComponent = m_Zelda->GetComponent<TransformComponent>();
	zeldaTransformComponent->SetPosition({ -100.f, 0.f, 0.f });
	zeldaTransformComponent->SetScale({ .5f, .5f, .5f });
	auto zeldaRenderComponent = m_Zelda->GetComponent<RenderComponent>();
	zeldaRenderComponent->SetVertexShaderKey(L"../Shaders/StaticVertexShader.hlsl");
	zeldaRenderComponent->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");

	m_Dancer4->AddComponent<TransformComponent>();
	m_Dancer4->AddComponent<RenderComponent>();
	m_Dancer4->AddComponent<AnimationComponent>();
	m_Dancer4->Load("../FBX/Hip Hop Dancing.fbx");
	auto dancer4TransformComponent = m_Dancer4->GetComponent<TransformComponent>();
	dancer4TransformComponent->SetPosition({ 0.f, 0.f, 0.f });
	dancer4TransformComponent->SetScale({ 1.f, 1.f, 1.f });
	auto dancer4RenderComponent = m_Dancer4->GetComponent<RenderComponent>();
	dancer4RenderComponent->SetVertexShaderKey(L"../Shaders/SkeletalVertexShader.hlsl");
	dancer4RenderComponent->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	auto dancer4AnimationComponent = m_Dancer4->GetComponent<AnimationComponent>();
	dancer4AnimationComponent->SelectAnimation("mixamo.com");
	dancer4AnimationComponent->SetAnimationSpeed(1.f);
	dancer4AnimationComponent->SetIsAnimationLoop(true);


	//m_Tree = new Model;
	//m_Soldier = new Model;
	//m_Walker = new Model;
	//m_Dancer1 = new Model;
	//m_Dancer2 = new Model;
	//m_Dancer3 = new Model;

	//m_Tree->Load("../FBX/tree.fbx");
	//m_Tree->SetPosition({ 0.f, 0.f, 0.f });
	//m_Tree->SetScale({ 60.f, 60.f, 60.f });
	//m_Tree->SetVectexShaderKey(L"../Shaders/StaticVertexShader.hlsl");
	//m_Tree->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");

	//m_Soldier->Load("../FBX/Character.fbx");
	//m_Soldier->SetPosition({ 100.f, 0.f, 0.f });
	//m_Soldier->SetScale({ 0.5f, 0.5f, 0.5f });
	//m_Soldier->SetVectexShaderKey(L"../Shaders/StaticVertexShader.hlsl");
	//m_Soldier->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");

	//m_Walker->Load("../FBX/AnimationModel.fbx");
	//m_Walker->SetPosition({ 0.f, 0.f, -100.f });
	//m_Walker->SetScale({ .1f, .1f, .1f });
	//m_Walker->SetVectexShaderKey(L"../Shaders/StaticVertexShader.hlsl");
	//m_Walker->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	//m_Walker->SelectAnimation("Scene");
	//m_Walker->SetAnimationSpeed(1.f);
	//m_Walker->SetIsAnimationLoop(true);

	//m_Dancer1->Load("../FBX/SkinningTest.fbx");
	//m_Dancer1->SetPosition({ -50.f, 20.f, -100.f });
	//m_Dancer1->SetScale({ .2f, .2f, .2f });
	//m_Dancer1->SetVectexShaderKey(L"../Shaders/SkeletalVertexShader.hlsl");
	//m_Dancer1->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	//m_Dancer1->SelectAnimation("mixamo.com");
	//m_Dancer1->SetAnimationSpeed(1.f);
	//m_Dancer1->SetIsAnimationLoop(true);

	//m_Dancer2->Load("../FBX/Breakdance 1990.fbx");
	//m_Dancer2->SetPosition({ -100.f, 20.f, -100.f });
	//m_Dancer2->SetScale({ .2f, .2f, .2f });
	//m_Dancer2->SetVectexShaderKey(L"../Shaders/SkeletalVertexShader.hlsl");
	//m_Dancer2->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	//m_Dancer2->SelectAnimation("mixamo.com");
	//m_Dancer2->SetAnimationSpeed(1.f);
	//m_Dancer2->SetIsAnimationLoop(true);

	//m_Dancer3->Load("../FBX/Rumba Dancing.fbx");
	//m_Dancer3->SetPosition({ 50.f, 20.f, -100.f });
	//m_Dancer3->SetScale({ .2f, .2f, .2f });
	//m_Dancer3->SetVectexShaderKey(L"../Shaders/SkeletalVertexShader.hlsl");
	//m_Dancer3->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	//m_Dancer3->SelectAnimation("mixamo.com");
	//m_Dancer3->SetAnimationSpeed(1.f);
	//m_Dancer3->SetIsAnimationLoop(true);

	//m_Dancer4->Load("../FBX/Hip Hop Dancing.fbx");
	//m_Dancer4->SetPosition({ 100.f, 20.f, -100.f });
	//m_Dancer4->SetScale({ .2f, .2f, .2f });
	//m_Dancer4->SetVectexShaderKey(L"../Shaders/SkeletalVertexShader.hlsl");
	//m_Dancer4->SetPixelShaderKey(L"../Shaders/PixelShader.hlsl");
	//m_Dancer4->SelectAnimation("mixamo.com");
	//m_Dancer4->SetAnimationSpeed(1.f);
	//m_Dancer4->SetIsAnimationLoop(true);

	return S_OK;
}

void Demo::Finalize()
{
	CleanupImGui();

	SAFE_DELETE(m_Camera);

	//SAFE_DELETE(m_Gun);
	//SAFE_DELETE(m_Tree);
	//SAFE_DELETE(m_Zelda);
	//SAFE_DELETE(m_Soldier);
	//SAFE_DELETE(m_Walker);
	//SAFE_DELETE(m_Dancer1);
	//SAFE_DELETE(m_Dancer2);
	//SAFE_DELETE(m_Dancer3);
	//SAFE_DELETE(m_Dancer4);

	Yngine::Finalize();
}

void Demo::RenderImGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	{
		ImGui::Begin("Cube Control");

		ImGui::Text("Cube1 Position");

		ImGui::SliderFloat("X", &m_ActorTranslate.x, -600.0f, 600.0f);
		ImGui::SliderFloat("Y", &m_ActorTranslate.y, -600.0f, 600.0f);
		ImGui::SliderFloat("Z", &m_ActorTranslate.z, -600.0f, 600.0f);
		ImGui::SliderFloat("Yaw", &m_ActorRotation.y, -180.0f, 180.0f);
		ImGui::SliderFloat("Pitch", &m_ActorRotation.x, -180.0f, 180.0f);
		ImGui::SliderFloat("Roll", &m_ActorRotation.z, -180.0f, 180.0f);

		auto TC = m_Dancer4->GetComponent<TransformComponent>();
		//TC->SetPosition(m_ActorTranslate);
		TC->SetRotation(m_ActorRotation);
		TC = m_Zelda->GetComponent<TransformComponent>();
		//TC->SetPosition(m_ActorTranslate);
		TC->SetRotation(m_ActorRotation);
		TC = m_Gun->GetComponent<TransformComponent>();
		TC->SetRotation(m_ActorRotation);
		TC->SetPosition(m_ActorTranslate);

		//ImGui::SliderFloat("Specular Power", &m_Zelda->GetSpecularPower(), 0.0f, 4000.0f);

		ImGui::End();
	}
	{
		ImGui::Begin("Camera Control");

		ImGui::Text("Camera Position");
		ImGui::SliderFloat("X", &(m_Camera->GetCameraPosition().x), -600.0f, 600.0f);
		ImGui::SliderFloat("Y", &(m_Camera->GetCameraPosition().y), -600.0f, 600.0f);
		ImGui::SliderFloat("Z", &(m_Camera->GetCameraPosition().z), -600.0f, 600.0f);

		ImGui::Text("Camera FOV");
		ImGui::SliderFloat("FOV", &(m_Camera->GetFOV()), 1.0f, 179.0f); // FOV를 조정합니다.

		ImGui::Text("Camera Near/Far");
		ImGui::SliderFloat("Near Plane", &(m_Camera->GetNearPlane()), 1.f, 10.0f); // 근접 평면을 조정합니다.
		ImGui::SliderFloat("Far Plane", &(m_Camera->GetFarPlane()), 10.1f, 10000.0f); // 원격 평면을 조정합니다.

		ImGui::End();
	}

	{
		ImGui::Begin("Light Control");

		ImGui::Text("Light Direction");
		ImGui::SliderFloat("X", &m_DirectLight.lightDir.x, 1.f, -1.f);
		ImGui::SliderFloat("Y", &m_DirectLight.lightDir.y, 1.f, -1.f);
		ImGui::SliderFloat("Z", &m_DirectLight.lightDir.z, 1.f, -1.f);
		ImGui::Text("Light Color");
		ImGui::SliderFloat("R", &m_DirectLight.lightDiffuse.x, 0.0f, 1.0f); // 원격 평면을 조정합니다.
		ImGui::SliderFloat("G", &m_DirectLight.lightDiffuse.y, 0.0f, 1.0f); // 원격 평면을 조정합니다.
		ImGui::SliderFloat("B", &m_DirectLight.lightDiffuse.z, 0.0f, 1.0f); // 원격 평면을 조정합니다.
		ImGui::End();

	}

	{
		ImGui::Begin("Delta Time");
		ImGui::Text("Delta Time: %.6f seconds", m_Timer->GetDeltaTime());
		ImGui::End();
	}

	{
		ImGui::Begin("Animation Control");

		//ImGui::SliderFloat("Animation Speed", &m_Walker->GetAnimationSpeed(), 0.01f, 5.00f);

		//float Speed = m_Walker->GetAnimationSpeed();

		//m_Dancer1->SetAnimationSpeed(Speed);
		//m_Dancer2->SetAnimationSpeed(Speed);
		//m_Dancer3->SetAnimationSpeed(Speed);
		//m_Dancer4->SetAnimationSpeed(Speed);

		//ImGui::SliderFloat("Specular Power", &m_Zelda->GetSpecularPower(), 0.0f, 4000.0f);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Demo::InitImGui()
{
	// ImGui 초기화
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// ImGui 스타일 및 테마 설정 (선택 사항)
	ImGui::StyleColorsDark();

	// ImGui 플랫폼/렌더러 초기화
	ImGui_ImplWin32_Init(m_hWnd);
	ImGui_ImplDX11_Init(m_D3DEngine->GetDevice(), m_D3DEngine->GetDeviceContext());
}

void Demo::CleanupImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Demo::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	return Yngine::WndProc(hWnd, message, wParam, lParam);
}



//// 구체의 수평 및 수직 분할 수
//int numSegments = 64;
//int numStacks = 32;
//
//// 구의 반지름
//float radius = 0.5f;
//
//// 그라데이션 시작 색상과 끝 색상 정의
////DirectX::SimpleMath::Vector4 baseColor(1.0f, 1.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 baseColor1(1.0f, 1.0f, 0.0f, 1.0f);
////DirectX::SimpleMath::Vector4 baseColor2(0.0f, 1.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 baseColor3(1.0f, 0.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 endColor(1.0f, 1.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 endColor1(0.0f, 1.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 endColor2(1.0f, 0.0f, 1.0f, 1.0f);
////DirectX::SimpleMath::Vector4 endColor3(1.0f, 1.0f, 0.0f, 1.0f);
//
//// 각 구체의 정점 생성
//for (int i = 0; i <= numStacks; ++i) {
//	float phi = DirectX::XM_PI * static_cast<float>(i) / numStacks;
//	float y = radius * cos(phi);
//
//	for (int j = 0; j <= numSegments; ++j) {
//		float theta = 2.0f * DirectX::XM_PI * static_cast<float>(j) / numSegments;
//		float x = radius * sin(phi) * cos(theta);
//		float z = radius * sin(phi) * sin(theta);
//
//		// 각 구체에 맞는 보간된 색상 계산
//		//DirectX::SimpleMath::Vector4 interpolatedColor1 = baseColor1 * (1.0f - static_cast<float>(j) / numSegments) + endColor1 * (static_cast<float>(j) / numSegments);
//		//DirectX::SimpleMath::Vector4 interpolatedColor2 = baseColor2 * (1.0f - static_cast<float>(j) / numSegments) + endColor2 * (static_cast<float>(j) / numSegments);
//		//DirectX::SimpleMath::Vector4 interpolatedColor3 = baseColor3 * (1.0f - static_cast<float>(j) / numSegments) + endColor3 * (static_cast<float>(j) / numSegments);
//		//DirectX::SimpleMath::Vector4 interpolatedColor = baseColor * (1.0f - static_cast<float>(j) / numSegments) + endColor * (static_cast<float>(j) / numSegments);
//
//		// 정점을 각 구체에 맞는 벡터에 추가
//		//Vertex v1(Vector3(x, y, z), Vector4(interpolatedColor1));
//		//Vertex v2(Vector3(x, y, z), Vector4(interpolatedColor2));
//		//Vertex v3(Vector3(x, y, z), Vector4(interpolatedColor3));
//		Vertex v(Vector3(x, y, z), m_LightSource.lightColor);
//
//		//m_Cube1.vertices.push_back(v1);
//		//m_Cube2.vertices.push_back(v2);
//		//m_Cube3.vertices.push_back(v3);
//		m_LightSource.vertices.push_back(v);
//	}
//}
//
//// 각 구체의 인덱스 생성 (삼각형 스트립을 사용)
//for (int i = 0; i < numStacks; ++i) {
//	for (int j = 0; j <= numSegments; ++j) {
//		int vertexIndex1 = i * (numSegments + 1) + j;
//		int vertexIndex2 = (i + 1) * (numSegments + 1) + j;
//
//		//m_Cube1.indices.push_back(vertexIndex1);
//		//m_Cube1.indices.push_back(vertexIndex2);
//
//		//m_Cube2.indices.push_back(vertexIndex1);
//		//m_Cube2.indices.push_back(vertexIndex2);
//
//		//m_Cube3.indices.push_back(vertexIndex1);
//		//m_Cube3.indices.push_back(vertexIndex2);
//		m_LightSource.indices.push_back(vertexIndex1);
//		m_LightSource.indices.push_back(vertexIndex2);
//	}
//}

	//Vertex point1_1(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_2(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_3(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_4(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));

	//Vertex point1_5(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_6(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_7(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_8(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));

	//Vertex point1_9(Vector3(-1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
	//Vertex point1_10(Vector3(-1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
	//Vertex point1_11(Vector3(-1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
	//Vertex point1_12(Vector3(-1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));

	//Vertex point1_13(Vector3(1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	//Vertex point1_14(Vector3(1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	//Vertex point1_15(Vector3(1.0f, 1.0f, -1.0f), Vector2(0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
	//Vertex point1_16(Vector3(1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));

	//Vertex point1_17(Vector3(-1.0f, -1.0f, -1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_18(Vector3(1.0f, -1.0f, -1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_19(Vector3(1.0f, 1.0f, -1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f));
	//Vertex point1_20(Vector3(-1.0f, 1.0f, -1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f));

	//Vertex point1_21(Vector3(-1.0f, -1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f));
	//Vertex point1_22(Vector3(1.0f, -1.0f, 1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f));
	//Vertex point1_23(Vector3(1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f));
	//Vertex point1_24(Vector3(-1.0f, 1.0f, 1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(-1.0f, 0.0f, 0.0f));


	//m_Cube1.vertices.push_back(point1_1);
	//m_Cube1.vertices.push_back(point1_2);
	//m_Cube1.vertices.push_back(point1_3);
	//m_Cube1.vertices.push_back(point1_4);
	//m_Cube1.vertices.push_back(point1_5);
	//m_Cube1.vertices.push_back(point1_6);
	//m_Cube1.vertices.push_back(point1_7);
	//m_Cube1.vertices.push_back(point1_8);
	//m_Cube1.vertices.push_back(point1_9);
	//m_Cube1.vertices.push_back(point1_10);
	//m_Cube1.vertices.push_back(point1_11);
	//m_Cube1.vertices.push_back(point1_12);
	//m_Cube1.vertices.push_back(point1_13);
	//m_Cube1.vertices.push_back(point1_14);
	//m_Cube1.vertices.push_back(point1_15);
	//m_Cube1.vertices.push_back(point1_16);
	//m_Cube1.vertices.push_back(point1_17);
	//m_Cube1.vertices.push_back(point1_18);
	//m_Cube1.vertices.push_back(point1_19);
	//m_Cube1.vertices.push_back(point1_20);
	//m_Cube1.vertices.push_back(point1_21);
	//m_Cube1.vertices.push_back(point1_22);
	//m_Cube1.vertices.push_back(point1_23);
	//m_Cube1.vertices.push_back(point1_24);

	//m_Cube1.indices =
	//{
	//	3,1,0, 2,1,3,
	//	6,4,5, 7,4,6,
	//	11,9,8, 10,9,11,
	//	14,12,13, 15,12,14,
	//	19,17,16, 18,17,19,
	//	22,20,21, 23,20,22
	//};