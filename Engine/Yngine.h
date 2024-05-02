#pragma once
#include <Windows.h>

class D3D;
class TimeSystem;

class Yngine
{
public:
	Yngine();
	virtual ~Yngine();

	static Yngine* m_pInstance;

protected:
	/*Window Resolution*/
	const int m_ResolutionWidth = 1920;
	const int m_ResolutionHeight = 1080;

	/*Windows*/
	HWND m_hWnd;
	MSG m_Msg;

protected:
	D3D* m_D3DEngine;
	TimeSystem* m_Timer;

protected:
	virtual void UpdateAll();
	virtual void RenderAll();

public:
	/*Init*/
	virtual HRESULT Initialize(HINSTANCE hInst);

	/*Loop*/
	void Loop();

	/*unInit*/
	virtual void Finalize();

	/*Message Handler*/
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};