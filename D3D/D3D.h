#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <directxtk/simplemath.h>
#include "Mesh.h"
#include <memory>

class Actor;

struct CB_ViewProjectionMatrix
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};

struct CB_TransformMatrix
{
	DirectX::SimpleMath::Matrix WorldTransformMatrix;
};

class D3D
{
private:
	D3D();
	~D3D();
	static D3D* m_Instance;

private:
	HWND m_hWnd;
	// 렌더링 파이프라인을 구성하는 필수 객체의 인터페이스 ( 뎊스 스텐실 뷰도 있지만 아직 사용하지 않는다.)
	ID3D11Device* m_pDevice = nullptr;						// 디바이스
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// 즉시 디바이스 컨텍스트
	IDXGISwapChain* m_pSwapChain = nullptr;					// 스왑체인
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// 렌더링 타겟뷰
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰
	ID3D11BlendState* m_pAlphaBlendState = nullptr;  // 깊이값 처리를 위한 뎊스스텐실 뷰

	// 렌더링 파이프라인에 적용하는 객체와 정보
	std::unordered_map<std::wstring, ID3D11VertexShader*> m_vertexShaders;
	std::unordered_map<std::wstring, ID3D11PixelShader*> m_pixelShaders;
	std::unordered_map<std::wstring, ID3DBlob*> m_vertexShaderBytecodes;
	std::unordered_map<std::wstring, ID3D11InputLayout*> m_pInputLayouts;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_Textures;
	std::unordered_map<std::wstring, ID3D11SamplerState*> m_SamplerLinears;

	DirectX::SimpleMath::Matrix m_View;				// 뷰좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix m_Projection;		// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	ID3D11Buffer* m_LightConstantBuffer;
	ID3D11Buffer* m_ViewProjConstantBuffer;
	ID3D11Buffer* m_TransformMatrixConstantBuffer;
	ID3D11Buffer* m_MatrixPalleteConstantBuffer;

public:
	static D3D* GetInstance();

	bool InitD3D(HWND hWnd, int ResolutionWidth, int ResolutionHeight);
	void UninitD3D();

	void PrepareRendering();
	void DrawVertices(const std::vector<VERTEX>& vertices);
	void DrawIndex(const std::vector<UINT>& indices);
	void PresentFrame();
	void Render(std::weak_ptr<Actor> model);

	ID3D11VertexShader* LoadVertexShader(const std::wstring& vectexShaderFilePath);
	ID3D11PixelShader* LoadPixelShader(const std::wstring& pixelShaderFilePath);
	ID3D11InputLayout* LoadInputLayout(const std::wstring& vectexShaderFilePath);
	ID3D11Buffer* CreateVertexBuffer(const std::vector<VERTEX>& vertices);
	ID3D11Buffer* CreateIndexBuffer(const std::vector<UINT>& indices);
	ID3D11Buffer* CreateConstantBuffer(UINT cbSize);

	ID3D11ShaderResourceView* LoadTexture(const std::wstring& TextureFilePath);
	ID3D11SamplerState* CreateSampler(const std::wstring& SamplerKey);

	void InitPipeline();

	void SetPipelineStage(std::weak_ptr<Actor> model);


	void UpdateLightConstantBuffer(const CB_Light& cbLight);
	void UpdateViewProjConstantBuffer(const Matrix& view, const Matrix& proj);
	void UpdateTransformMatrixConstantBuffer(const Matrix& tm);
	void UpdateMatrixPalleteConstantBuffer(const CB_MatrixPallete& mp);

public:
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_pRenderTargetView; }


	void SetViewMatrix(DirectX::SimpleMath::Matrix viewmatrix) { m_View = viewmatrix; }
	void SetProjectionMatrix(DirectX::SimpleMath::Matrix projectionmatrix) { m_Projection = projectionmatrix; }

	DirectX::SimpleMath::Matrix GetViewMatrix() { return m_View; }
	DirectX::SimpleMath::Matrix GetProjectionMatrix() { return m_Projection; }
};

