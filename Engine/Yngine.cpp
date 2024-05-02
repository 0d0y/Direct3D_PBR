#include "pch.h"
#include "Helper.h"
#include "Yngine.h"
#include "../D3D/D3D.h"
#include "TimeSystem.h"

Yngine* Yngine::m_pInstance = nullptr;

LRESULT CALLBACK DefaultWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return  Yngine::m_pInstance->WndProc(hWnd, message, wParam, lParam);
}

Yngine::Yngine()
	: m_hWnd()
	, m_Msg()
	, m_D3DEngine(nullptr)
{
	Yngine::m_pInstance = this;
}

Yngine::~Yngine()
{
}

void Yngine::UpdateAll()
{
	m_Timer->Update();
}

void Yngine::RenderAll()
{

}

HRESULT Yngine::Initialize(HINSTANCE hInst)
{
	wchar_t AppName[] = L"Yn";
	/*윈도우 클래스 생성*/
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = DefaultWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = AppName;

	/*윈도우 클래스 등록*/
	RegisterClass(&wndclass);

	RECT rcClient = { 0, 0, (LONG)m_ResolutionWidth, (LONG)m_ResolutionHeight };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);

	/*윈도우 생성*/
	m_hWnd = CreateWindow(
		AppName,
		AppName,
		WS_OVERLAPPEDWINDOW,
		100, 100,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		NULL, NULL, hInst, NULL);


	if (!m_hWnd)
		return FALSE;

	/*윈도우 화면에 띄우기*/
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	/*시스템 초기화*/
	//그래픽스
	m_D3DEngine = D3D::GetInstance();
	m_D3DEngine->InitD3D(m_hWnd, m_ResolutionWidth, m_ResolutionHeight);

	m_Timer = new TimeSystem;

	return S_OK;
}

void Yngine::Loop()
{
	while (true)
	{
		if (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_QUIT) break;

			DispatchMessage(&m_Msg);
		}
		else
		{
			UpdateAll();
			RenderAll();
		}
	}
}

void Yngine::Finalize()
{
	SAFE_DELETE(m_Timer);
	m_D3DEngine->UninitD3D();
	delete this;
}

LRESULT CALLBACK Yngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
