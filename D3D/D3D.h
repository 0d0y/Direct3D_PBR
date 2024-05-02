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
	// ������ ������������ �����ϴ� �ʼ� ��ü�� �������̽� ( �X�� ���ٽ� �䵵 ������ ���� ������� �ʴ´�.)
	ID3D11Device* m_pDevice = nullptr;						// ����̽�
	ID3D11DeviceContext* m_pDeviceContext = nullptr;		// ��� ����̽� ���ؽ�Ʈ
	IDXGISwapChain* m_pSwapChain = nullptr;					// ����ü��
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;	// ������ Ÿ�ٺ�
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��
	ID3D11BlendState* m_pAlphaBlendState = nullptr;  // ���̰� ó���� ���� �X�����ٽ� ��

	// ������ ���������ο� �����ϴ� ��ü�� ����
	std::unordered_map<std::wstring, ID3D11VertexShader*> m_vertexShaders;
	std::unordered_map<std::wstring, ID3D11PixelShader*> m_pixelShaders;
	std::unordered_map<std::wstring, ID3DBlob*> m_vertexShaderBytecodes;
	std::unordered_map<std::wstring, ID3D11InputLayout*> m_pInputLayouts;
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_Textures;
	std::unordered_map<std::wstring, ID3D11SamplerState*> m_SamplerLinears;

	DirectX::SimpleMath::Matrix m_View;				// ����ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix m_Projection;		// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

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

