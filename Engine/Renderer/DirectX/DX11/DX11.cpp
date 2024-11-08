#include "DX11.h"
#include <d3dcompiler.h>
#include "Vendor/stb/stb_image.h"

Engine::DX11::DX11()
	:pDevice(), pDeviceContext()
{
	unsigned int createDeviceFlags = 0;
	D3D_DRIVER_TYPE driverTypes[] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};

	unsigned int numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	unsigned int numFeatureLevels = ARRAYSIZE(featureLevels);
	D3D_DRIVER_TYPE activeDriverType;
	D3D_FEATURE_LEVEL activeFeatureLevel;
	HRESULT hr = S_OK;

	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0,
		D3D11_SDK_VERSION, &pDevice, &activeFeatureLevel, &pDeviceContext);

	// TODO: Investigate this code more to leverage it
	for (unsigned int driverTypeIndex = 0u; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		activeDriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(nullptr, activeDriverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &pDevice, &activeFeatureLevel, &pDeviceContext);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = D3D11CreateDevice(nullptr, activeDriverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
				D3D11_SDK_VERSION, &pDevice, &activeFeatureLevel, &pDeviceContext);
		}

		if (SUCCEEDED(hr))
			break;
	}

	pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory1);

}

Engine::DX11::~DX11()
{
	RELEASECOM(pRenderTargetView);
	RELEASECOM(pSwapChain);
	RELEASECOM(pDXGIFactory1);
	RELEASECOM(pDXGIAdapter);
	RELEASECOM(pDXGIDevice);
	RELEASECOM(pDeviceContext);
	RELEASECOM(pDevice);
}

ID3D11Buffer* Engine::DX11::CreateVertexBuffer(unsigned int _size, void* _vertices)
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	CLEARMEM(vertexBufferDesc);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = _size;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	CLEARMEM(vertexBufferData);
	vertexBufferData.pSysMem = _vertices;
	pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &buffer);

	return buffer;
}

ID3D11Buffer* Engine::DX11::CreateIndexBuffer(unsigned int _size, void* _indices)
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC indexBufferDesc = { 0 };
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = _size;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;	// CPU does not have read nor write access

	D3D11_SUBRESOURCE_DATA IndexBufferData = { 0 };
	IndexBufferData.pSysMem = _indices;
	pDevice->CreateBuffer(&indexBufferDesc, &IndexBufferData, &buffer);

	return buffer;
}

ID3D11Buffer* Engine::DX11::CreateConstantBuffer(unsigned int _resourceSize, void* _resource)
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = _resourceSize;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA cbResource = {};
	cbResource.pSysMem = _resource;
	cbResource.SysMemPitch = 0;
	cbResource.SysMemSlicePitch = 0;

	pDevice->CreateBuffer(&cbDesc, &cbResource, &buffer);

	return buffer;
}

ID3D11InputLayout* Engine::DX11::CreateInputLayout(ID3D10Blob* const _vertexShaderBuffer)
{
	ID3D11InputLayout* inputLayout = nullptr;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	pDevice->CreateInputLayout(layout, ARRAYSIZE(layout), _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), &inputLayout);

	return inputLayout;
}

ID3D10Blob* Engine::DX11::LoadShaderData(const wchar_t* _filePath)
{
	ID3D10Blob* pBlob = nullptr;

	D3DReadFileToBlob(_filePath, &pBlob);

	return pBlob;
}

ID3D11VertexShader* Engine::DX11::CreateVertexShader(ID3D10Blob* _vsdata)
{
	ID3D11VertexShader* vs = nullptr;

	pDevice->CreateVertexShader(_vsdata->GetBufferPointer(), _vsdata->GetBufferSize(), nullptr, &vs);

	return vs;
}

ID3D11PixelShader* Engine::DX11::CreatePixelShader(ID3D10Blob* _psdata)
{
	ID3D11PixelShader* ps = nullptr;
	
	pDevice->CreatePixelShader(_psdata->GetBufferPointer(), _psdata->GetBufferSize(), nullptr, &ps);
	
	return ps;
}

ID3D11SamplerState* Engine::DX11::CreateSamplerState()
{
	ID3D11SamplerState* pSamplerState = nullptr;
	D3D11_SAMPLER_DESC sampDesc;
	CLEARMEM(sampDesc);
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	DirectX::XMFLOAT4 bclr = { 1.0f, 1.0f, 1.0f, 0.0f };
	memcpy_s(&sampDesc.BorderColor, sizeof(sampDesc.BorderColor), &bclr, sizeof(bclr));
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pDevice->CreateSamplerState(&sampDesc, &pSamplerState);

	return pSamplerState;
}

ID3D11BlendState* Engine::DX11::CreateBlendState()
{
	ID3D11BlendState* pBlendState = nullptr;
	D3D11_BLEND_DESC blendStateDesc = {};
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pDevice->CreateBlendState(&blendStateDesc, &pBlendState);

	return pBlendState;
}

void Engine::DX11::CreateRenderTargetView(DXGI_SWAP_CHAIN_DESC& _desc)
{
	pDXGIFactory1->CreateSwapChain(pDevice, &_desc, &pSwapChain);
	pDXGIFactory1->MakeWindowAssociation(_desc.OutputWindow, DXGI_MWA_NO_ALT_ENTER);

	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (pBackBuffer)
	{
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		RELEASECOM(pBackBuffer);
	}

}

void Engine::DX11::CreateRenderTargetView(HWND _hwnd)
{
	RECT rc;
	GetClientRect(_hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC mSwapChainDesc = { 0 };
	mSwapChainDesc.BufferDesc.Width = width;
	mSwapChainDesc.BufferDesc.Height = height;
	mSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	mSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	mSwapChainDesc.SampleDesc.Count = 1;
	mSwapChainDesc.SampleDesc.Quality = 0;
	mSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	mSwapChainDesc.BufferCount = 1;
	mSwapChainDesc.OutputWindow = _hwnd;
	mSwapChainDesc.Windowed = true;
	mSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	mSwapChainDesc.Flags = 0;

	pDXGIFactory1->CreateSwapChain(pDevice, &mSwapChainDesc, &pSwapChain);
	pDXGIFactory1->MakeWindowAssociation(mSwapChainDesc.OutputWindow, DXGI_MWA_NO_ALT_ENTER);

	ID3D11Texture2D* pBackBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (pBackBuffer)
	{
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
		RELEASECOM(pBackBuffer);
	}
}

ID3D11Texture2D* Engine::DX11::CreateTexture(D3D11_TEXTURE2D_DESC& _desc, D3D11_SUBRESOURCE_DATA& _data)
{
	ID3D11Texture2D* pTexture = nullptr;

	pDevice->CreateTexture2D(&_desc, &_data, &pTexture);

	return pTexture;
}

ID3D11Texture2D* Engine::DX11::CreateTexture(const char* _filePath, DXGI_FORMAT _format /*= DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/, unsigned int _sampleCount /*= 1*/, unsigned int _sampleQuality /*= 1*/, unsigned int _mipLevel /*= 1*/, unsigned int _arraySize /*= 1*/)
{
	int twidth = 0;
	int theight = 0;
	int tchannels = 0;
	unsigned char* pTextureFileData = stbi_load(_filePath, &twidth, &theight, &tchannels, 4);
	D3D11_SUBRESOURCE_DATA wallData = {};
	wallData.pSysMem = pTextureFileData;
	wallData.SysMemPitch = twidth * 4;

	D3D11_TEXTURE2D_DESC wallDesc = {};
	wallDesc.Width = twidth;
	wallDesc.Height = theight;
	wallDesc.MipLevels = 1;
	wallDesc.ArraySize = 1;
	wallDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	wallDesc.SampleDesc.Count = 1;
	wallDesc.SampleDesc.Quality = 0;
	wallDesc.Usage = D3D11_USAGE_IMMUTABLE;
	wallDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* pTexture = nullptr;
	pDevice->CreateTexture2D(&wallDesc, &wallData, &pTexture);
	stbi_image_free(pTextureFileData);

	return pTexture;
}

ID3D11ShaderResourceView* Engine::DX11::CreateShaderResourceView(ID3D11Texture2D* _texture)
{
	ID3D11ShaderResourceView* pResource = nullptr;

	pDevice->CreateShaderResourceView(_texture, nullptr, &pResource);

	return pResource;
}

void Engine::DX11::ClearScreen(DirectX::XMFLOAT4& _clearColor)
{
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, &_clearColor.x);
}

void Engine::DX11::SetViewport(const D3D11_VIEWPORT* _viewport, int _numOfViewports)
{
	pDeviceContext->RSSetViewports(_numOfViewports, _viewport);
}

void Engine::DX11::SetRenderTarget(ID3D11DepthStencilView* _depthStencil, int _numOfTargets /*= 1*/)
{
	pDeviceContext->OMSetRenderTargets(_numOfTargets, &pRenderTargetView, _depthStencil);
	
}

void Engine::DX11::SetBlendState(ID3D11BlendState* _blendState)
{
	pDeviceContext->OMSetBlendState(_blendState, nullptr, 0xffffffff);
}

void Engine::DX11::SetInputLayout(D3D_PRIMITIVE_TOPOLOGY _topology, ID3D11InputLayout* _inputLayout)
{
	pDeviceContext->IASetPrimitiveTopology(_topology);
	pDeviceContext->IASetInputLayout(_inputLayout);
}

void Engine::DX11::SetConstantBuffer(ID3D11Buffer*& _constantBuffer, int _slot, int _count /*= 1*/)
{
	pDeviceContext->VSSetConstantBuffers(_slot, _count, &_constantBuffer);
}

void Engine::DX11::SetVertexShader(ID3D11VertexShader* _vertexShader)
{
	pDeviceContext->VSSetShader(_vertexShader, nullptr, 0);
}

void Engine::DX11::SetPixelShader(ID3D11PixelShader* _pixelShader)
{
	pDeviceContext->PSSetShader(_pixelShader, nullptr, 0);
}

void Engine::DX11::SetSampler(ID3D11SamplerState*& _sampler, int _startSlot /*= 0*/, int _numOfSamplers /*= 1*/)
{
	pDeviceContext->PSSetSamplers(_startSlot, _numOfSamplers, &_sampler);
}

void Engine::DX11::SetTexture(ID3D11ShaderResourceView*& _texture, int _slot, int _numOfTextures /*= 1*/)
{
	pDeviceContext->PSSetShaderResources(_slot, _numOfTextures, &_texture);
}

void Engine::DX11::SetVertexBuffer(ID3D11Buffer*& _buffer, unsigned  int _stride, unsigned int _offset, int _slot, int _count /*= 1*/)
{
	pDeviceContext->IASetVertexBuffers(_slot, _count, &_buffer, &_stride, &_offset);
}

void Engine::DX11::SetIndexBuffer(ID3D11Buffer*& _buffer, DXGI_FORMAT _format /*= DXGI_FORMAT::DXGI_FORMAT_R32_UINT*/, int _offset /*= 0*/)
{
	pDeviceContext->IASetIndexBuffer(_buffer, _format, _offset);
}

void Engine::DX11::DrawInstanced(int _indicesPerInstance, int _numOfInstances, int _startIndex /*= 0*/, int _startInstanceID /*= 0*/, int _baseVertexLocation /*= 0*/)
{
	pDeviceContext->DrawIndexedInstanced(_indicesPerInstance, _numOfInstances, _startIndex, _startInstanceID, _baseVertexLocation);
}

void Engine::DX11::Present()
{
	pSwapChain->Present(1, 0);
}

