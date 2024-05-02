#include "../Engine/Helper.h"
#include "D3D.h"
#include "directxtk/WICTextureLoader.h"
#include "Model.h"
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include "../Engine/Actor.h"
#include "../Engine/RenderComponent.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib,"d3dcompiler.lib")

D3D* D3D::m_Instance = nullptr;

#define USE_FLIPMODE 1			//��� �޼����� ���ַ��� Flip ��带 ����Ѵ�.

D3D::D3D()
	: m_hWnd(nullptr)
{
}

D3D::~D3D()
{
	UninitD3D();
	delete m_Instance;
}


void D3D::PrepareRendering()
{
	// Flip��忡���� �������� ���� �ʿ�
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	// ȭ�� ĥ�ϱ�.
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0); // �������� 1.0f�� �ʱ�ȭ.
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::MediumSeaGreen);
}

void D3D::DrawVertices(const std::vector<VERTEX>& vertices)
{
	UINT vertextBufferStride = sizeof(VERTEX);
	UINT vertextBufferOffset = 0;
	UINT vertexCount = static_cast<UINT>(vertices.size());

	m_pDeviceContext->Draw(vertexCount, vertextBufferOffset);
}

void D3D::DrawIndex(const std::vector<UINT>& indices)
{
	m_pDeviceContext->DrawIndexed(indices.size(), 0, 0);
}

void D3D::PresentFrame()
{
	// ����ü�� ��ü
	m_pSwapChain->Present(0, 0);
}

void D3D::Render(std::weak_ptr<Actor> model)
{
	SetPipelineStage(model);
	model.lock()->Render(m_pDeviceContext);
}

ID3D11VertexShader* D3D::LoadVertexShader(const std::wstring& vectexShaderFileName)
{
	auto it = m_vertexShaders.find(vectexShaderFileName);
	if (it != m_vertexShaders.end())
	{
		return it->second;
	}

	ID3D11VertexShader* pVertexShader = nullptr;
	HRESULT hr = 0;
	ID3DBlob* shaderBlob = nullptr;

	HR_T(hr = CompileShaderFromFile(vectexShaderFileName.c_str(), "main", "vs_5_0", &shaderBlob));
	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, (char*)shaderBlob->GetBufferPointer(), "����.", MB_OK);
		SAFE_RELEASE(shaderBlob);	// ������ ���� �޼��� ���̻� �ʿ����
		return nullptr;
	}

	HR_T(hr = m_pDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pVertexShader));
	if (FAILED(hr))
	{
		SAFE_RELEASE(shaderBlob);
		return nullptr;
	}

	if (pVertexShader != nullptr)
	{
		m_vertexShaders[vectexShaderFileName] = pVertexShader;
		//��ǲ���̾ƿ��� ���� ����Ʈ�ڵ�ʿ� ���ؽ����̴� ����Ʈ�ڵ� ����
		m_vertexShaderBytecodes[vectexShaderFileName] = shaderBlob;
	}

	LoadInputLayout(vectexShaderFileName);

	return pVertexShader;
}

ID3D11PixelShader* D3D::LoadPixelShader(const std::wstring& pixelShaderFileName)
{
	auto it = m_pixelShaders.find(pixelShaderFileName);
	if (it != m_pixelShaders.end())
	{
		return it->second;
	}

	ID3D11PixelShader* pPixelShader = nullptr;
	HRESULT hr = 0;
	ID3DBlob* shaderBlob = nullptr;

	HR_T(hr = CompileShaderFromFile(pixelShaderFileName.c_str(), "main", "ps_5_0", &shaderBlob));
	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, (char*)shaderBlob->GetBufferPointer(), "����.", MB_OK);
		SAFE_RELEASE(shaderBlob);	// ������ ���� �޼��� ���̻� �ʿ����
		return nullptr;
	}

	HR_T(hr = m_pDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &pPixelShader));
	if (FAILED(hr))
	{
		SAFE_RELEASE(shaderBlob);
		return nullptr;
	}

	if (pPixelShader != nullptr)
	{
		m_pixelShaders[pixelShaderFileName] = pPixelShader;
	}

	SAFE_RELEASE(shaderBlob);

	return pPixelShader;
}

ID3D11InputLayout* D3D::LoadInputLayout(const std::wstring& vectexShaderFileName)
{
	HRESULT hr = 0;

	ID3D11InputLayout* pInputLayout = nullptr;

	auto it = m_pInputLayouts.find(vectexShaderFileName);
	if (it != m_pInputLayouts.end())
	{
		return it->second;
	}

	auto vit = m_vertexShaderBytecodes.find(vectexShaderFileName);
	if (vit == m_vertexShaderBytecodes.end())
	{
		return nullptr;
	}

	//���������ο� ���ε��� InputLayout ����
	D3D11_INPUT_ELEMENT_DESC layout[] =  // ��ǲ ���̾ƿ��� ���ؽ� ���̴��� �Է¹��� �������� ������ �����Ѵ�.
	{// SemanticName , SemanticIndex , Format , InputSlot , AlignedByteOffset , InputSlotClass , InstanceDataStepRate
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HR_T(hr = m_pDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
		vit->second->GetBufferPointer(), vit->second->GetBufferSize(), &pInputLayout));

	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, "��ǲ ���̾ƿ� ���� ����", "����.", MB_OK);
		return nullptr;
	}

	if (pInputLayout != nullptr)
	{
		m_pInputLayouts[vectexShaderFileName] = pInputLayout;
	}

	return pInputLayout;
}

ID3D11Buffer* D3D::CreateVertexBuffer(const std::vector<VERTEX>& vertices)
{
	HRESULT hr = 0; // �����.
	ID3D11Buffer* pVertexBuffer;

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = sizeof(VERTEX) * static_cast<UINT>(vertices.size());
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;

	// ���� ���� ����.
	D3D11_SUBRESOURCE_DATA vbData = {};
	vbData.pSysMem = vertices.data();
	HR_T(hr = m_pDevice->CreateBuffer(&vbDesc, &vbData, &pVertexBuffer));

	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, "���ؽ� ���� ���� ����", "����.", MB_OK);
		return nullptr;
	}

	return pVertexBuffer;
}

ID3D11Buffer* D3D::CreateIndexBuffer(const std::vector<UINT>& indices)
{
	HRESULT hr = 0; // �����
	ID3D11Buffer* pIndexBuffer = nullptr;

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices.data();

	HR_T(hr = m_pDevice->CreateBuffer(&indexBufferDesc, &initData, &pIndexBuffer));

	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, "�ε��� ���� ���� ����", "����.", MB_OK);
		return nullptr;
	}

	return pIndexBuffer;
}

ID3D11Buffer* D3D::CreateConstantBuffer(UINT cbSize)
{
	HRESULT hr = 0; // �����
	ID3D11Buffer* pConstantBuffer = nullptr;

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.ByteWidth = cbSize;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&constantBufferDesc, nullptr, &pConstantBuffer));

	if (FAILED(hr))
	{
		MessageBoxA(m_hWnd, "��� ���� ���� ����", "����.", MB_OK);
		return nullptr;
	}

	return pConstantBuffer;
}

ID3D11ShaderResourceView* D3D::LoadTexture(const std::wstring& TextureFilePath)
{

	auto it = m_Textures.find(TextureFilePath);
	if (it != m_Textures.end())
	{
		return it->second;
	}

	ID3D11ShaderResourceView* shaderResourceView = nullptr;

	HR_T(CreateWICTextureFromFile(m_pDevice, TextureFilePath.c_str(), nullptr, &shaderResourceView));

	if (shaderResourceView)
	{
		m_Textures[TextureFilePath] = shaderResourceView;
	}

	return shaderResourceView;
}

ID3D11SamplerState* D3D::CreateSampler(const std::wstring& SamplerKey)
{
	ID3D11SamplerState* sampler = nullptr;
	D3D11_SAMPLER_DESC sampDesc = {};
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR_T(m_pDevice->CreateSamplerState(&sampDesc, &sampler));
	m_SamplerLinears[SamplerKey] = sampler;

	return sampler;
}

void D3D::InitPipeline()
{
	HRESULT hr = 0; // �����.
	ID3D10Blob* errorMessage = nullptr;
	m_LightConstantBuffer = CreateConstantBuffer(sizeof(CB_Light));
	m_ViewProjConstantBuffer = CreateConstantBuffer(sizeof(CB_ViewProjectionMatrix));
	m_TransformMatrixConstantBuffer = CreateConstantBuffer(sizeof(CB_TransformMatrix));
	m_MatrixPalleteConstantBuffer = CreateConstantBuffer(sizeof(CB_MatrixPallete));
}

void D3D::SetPipelineStage(std::weak_ptr<Actor> model)
{
	// Draw�迭 �Լ��� ȣ���ϱ����� ������ ���������ο� �ʼ� �������� ������ �ؾ��Ѵ�.
	UINT vertextBufferStride = sizeof(VERTEX);					// ���ؽ� �ϳ��� ũ��.
	UINT vertextBufferOffset = 0;

	//ID3D11Buffer* pVertexBuffer = figure.meshes->GetVertexBuffer();
	//ID3D11Buffer* pIndexBuffer = figure.meshes->GetIndexBuffer();
	auto renderComponent = model.lock()->GetComponent<RenderComponent>();

	m_pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // ������ �̾ �׸� ��� ����.
	//m_pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &vertextBufferStride, &vertextBufferOffset);
	m_pDeviceContext->IASetInputLayout(LoadInputLayout(renderComponent->GetVertexShaderKey()));
	//m_pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	m_pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xFFFFFFFF);
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_ViewProjConstantBuffer);
	m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_TransformMatrixConstantBuffer);
	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_MatrixPalleteConstantBuffer);
	m_pDeviceContext->VSSetShader(LoadVertexShader(renderComponent->GetVertexShaderKey()), nullptr, 0);
	m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_LightConstantBuffer);
	m_pDeviceContext->PSSetShader(LoadPixelShader(renderComponent->GetPixelShaderKey()), nullptr, 0);
	//m_pDeviceContext->PSSetShaderResources(0, 1, &m_Textures[figure.textureKey]);
	//m_pDeviceContext->PSSetShaderResources(1, 1, &m_Textures[figure.normalMapKey]);
	//m_pDeviceContext->PSSetShaderResources(2, 1, &m_Textures[figure.roughnessMapKey]);
	m_pDeviceContext->PSSetSamplers(0, 1, &m_SamplerLinears[L"Default"]);

	//SAFE_RELEASE(pVertexBuffer);
	//SAFE_RELEASE(pIndexBuffer);

}

void D3D::UpdateTransformMatrixConstantBuffer(const Matrix& tm)
{
	if (m_TransformMatrixConstantBuffer)
	{
		CB_TransformMatrix cbtm;
		cbtm.WorldTransformMatrix = tm;
		m_pDeviceContext->UpdateSubresource(m_TransformMatrixConstantBuffer, 0, nullptr, &cbtm, 0, 0);
	}
}

void D3D::UpdateMatrixPalleteConstantBuffer(const CB_MatrixPallete& mp)
{
	if (m_MatrixPalleteConstantBuffer)
	{
		m_pDeviceContext->UpdateSubresource(m_MatrixPalleteConstantBuffer, 0, nullptr, &mp, 0, 0);
	}
}

void D3D::UpdateLightConstantBuffer(const CB_Light& cbLight)
{
	if (m_LightConstantBuffer)
	{
		m_pDeviceContext->UpdateSubresource(m_LightConstantBuffer, 0, nullptr, &cbLight, 0, 0);
	}
}

void D3D::UpdateViewProjConstantBuffer(const Matrix& view, const Matrix& proj)
{
	if (m_ViewProjConstantBuffer)
	{
		CB_ViewProjectionMatrix vpm;
		vpm.view = view;
		vpm.projection = proj;

		m_pDeviceContext->UpdateSubresource(m_ViewProjConstantBuffer, 0, nullptr, &vpm, 0, 0);
	}
}


D3D* D3D::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new D3D;
	}
	return m_Instance;
}

bool D3D::InitD3D(HWND hWnd, int ResolutionWidth, int ResolutionHeight)
{
	HRESULT hr = 0; // �����.

	m_hWnd = hWnd;

	// ����ü�� �Ӽ� ���� ����ü ����.
	DXGI_SWAP_CHAIN_DESC swapDesc = {};

	swapDesc.BufferCount = 2;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = m_hWnd;	// ����ü�� ����� â �ڵ� ��.
	swapDesc.Windowed = true;		// â ��� ���� ����.
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �����(�ؽ�ó)�� ����/���� ũ�� ����.
	swapDesc.BufferDesc.Width = ResolutionWidth;
	swapDesc.BufferDesc.Height = ResolutionHeight;
	// ȭ�� �ֻ��� ����.
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// ���ø� ���� ����.
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;


	UINT creationFlags = 0;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 1. ��ġ ����.   2.����ü�� ����. 3.��ġ ���ؽ�Ʈ ����.
	HR_T(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, NULL, NULL,
		D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

	// 4. ����Ÿ�ٺ� ����.  (����۸� �̿��ϴ� ����Ÿ�ٺ�)
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	HR_T(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture));
	HR_T(m_pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &m_pRenderTargetView));  // �ؽ�ó�� ���� ���� ����
	SAFE_RELEASE(pBackBufferTexture);	//�ܺ� ���� ī��Ʈ�� ���ҽ�Ų��.

	// ���� Ÿ���� ���� ��� ���������ο� ���ε��մϴ�.
	m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	// ����Ʈ ����.
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)ResolutionWidth;
	viewport.Height = (float)ResolutionHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pDeviceContext->RSSetViewports(1, &viewport);

	//6. �X��&���ٽ� �� ����
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = ResolutionWidth;
	descDepth.Height = ResolutionHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* textureDepthStencil = nullptr;
	HR_T(m_pDevice->CreateTexture2D(&descDepth, nullptr, &textureDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	HR_T(m_pDevice->CreateDepthStencilView(textureDepthStencil, &descDSV, &m_pDepthStencilView));
	SAFE_RELEASE(textureDepthStencil);

	//7. ���ĺ����� ���� ���� ���� ����
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = true; // ���� ��� ����
	//FinalRGB = SrcRGB * SrcBlend + DestRGB * DestBlend
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA; // SrcBlend�� SrcColor�� ���İ�
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //DestBlend�� 1 - SourceColor.a
	//FinalAlpha = (SrcAlpha * SrcBlendAlpha) + (DestAlpha * DestBlendAlpha)
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; //����Ÿ�ٿ� rgba ��� write
	blendDesc.RenderTarget[0] = rtBlendDesc;
	HR_T(m_pDevice->CreateBlendState(&blendDesc, &m_pAlphaBlendState));
	//m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	CreateSampler(L"Default");

	return true;
}

void D3D::UninitD3D()
{
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDepthStencilView);
	SAFE_RELEASE(m_pAlphaBlendState);

	SAFE_RELEASE(m_LightConstantBuffer);
	SAFE_RELEASE(m_ViewProjConstantBuffer);
	SAFE_RELEASE(m_TransformMatrixConstantBuffer);

	for (auto& pair : m_vertexShaders)
	{
		SAFE_RELEASE(pair.second);
	}
	for (auto& pair : m_pixelShaders)
	{
		SAFE_RELEASE(pair.second);
	}
	for (auto& pair : m_vertexShaderBytecodes)
	{
		SAFE_RELEASE(pair.second);
	}
	for (auto& pair : m_pInputLayouts)
	{
		SAFE_RELEASE(pair.second);
	}
	for (auto& pair : m_Textures)
	{
		SAFE_RELEASE(pair.second);
	}
	for (auto& pair : m_SamplerLinears)
	{
		SAFE_RELEASE(pair.second);
	}

#ifdef _DEBUG
	ID3D11Debug* debugDev = nullptr;
	m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
	if (debugDev)
	{
		debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debugDev->Release();
	}
#endif
}
