
#ifndef ENGINE_RENDERER_DIRECTX_DX11_H
#define ENGINE_RENDERER_DIRECTX_DX11_H

#include <d3d11.h>
#include <windows.h>
#include <DirectXMath.h>

#define RELEASECOM(x) if(x!=nullptr){x->Release();x=nullptr;}
#define CLEARMEM(x) ZeroMemory(&x, sizeof(x))

namespace Engine
{
	class DX11
	{
	public:
		DX11();
		~DX11();

		// ----- Buffers
		ID3D11Buffer* CreateVertexBuffer(unsigned int _size, void* _vertices);
		ID3D11Buffer* CreateIndexBuffer(unsigned int _size, void* _indices);
		ID3D11Buffer* CreateConstantBuffer(unsigned int _resourceSize, void* _resource);

		// ----- Layout
		ID3D11InputLayout* CreateInputLayout(ID3D10Blob* const _vertexShaderBuffer);

		// ----- Shaders
		ID3D10Blob* LoadShaderData(const wchar_t* _filePath);
		ID3D11VertexShader* CreateVertexShader(ID3D10Blob* _vsdata);
		ID3D11PixelShader* CreatePixelShader(ID3D10Blob* _psdata);

		// ----- States
		ID3D11SamplerState* CreateSamplerState();
		ID3D11BlendState* CreateBlendState();

		// ----- Swap Chain
		void CreateRenderTargetView(DXGI_SWAP_CHAIN_DESC& _desc);
		void CreateRenderTargetView(HWND _hwnd);

		// ----- Texture
		ID3D11Texture2D* CreateTexture(D3D11_TEXTURE2D_DESC& _desc, D3D11_SUBRESOURCE_DATA& _data);
		ID3D11Texture2D* CreateTexture(const char* _filePath, DXGI_FORMAT _format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, unsigned int _sampleCount = 1, unsigned int _sampleQuality = 1, unsigned int _mipLevel = 1, unsigned int _arraySize = 1);
		ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* _texture);

		// ----- Draw

		void ClearScreen(DirectX::XMFLOAT4& _clearColor);
		void SetViewport(const D3D11_VIEWPORT* _viewport, int _numOfViewports = 1);
		void SetRenderTarget(ID3D11DepthStencilView* _depthStencil, int _numOfTargets = 1);
		void SetBlendState(ID3D11BlendState* _blendState);
		void SetInputLayout(D3D_PRIMITIVE_TOPOLOGY _topology, ID3D11InputLayout* _inputLayout);
		void SetConstantBuffer(ID3D11Buffer*& _constantBuffer, int _slot, int _count = 1);
		void SetVertexShader(ID3D11VertexShader* _vertexShader);
		void SetPixelShader(ID3D11PixelShader* _pixelShader);
		void SetSampler(ID3D11SamplerState*& _sampler, int _startSlot = 0, int _numOfSamplers = 1);
		void SetTexture(ID3D11ShaderResourceView*& _texture, int _slot, int _numOfTextures = 1);
		void SetVertexBuffer(ID3D11Buffer*& _buffer, unsigned int _stride, unsigned int _offset, int _slot, int _count = 1);
		void SetIndexBuffer(ID3D11Buffer*& _buffer, DXGI_FORMAT _format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT, int _offset = 0);
		void DrawInstanced(int _indicesPerInstance, int _numOfInstances, int _startIndex = 0, int _startInstanceID = 0, int _baseVertexLocation = 0);
		void Present();

	private:
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pDeviceContext;
		IDXGISwapChain* pSwapChain;
		ID3D11RenderTargetView* pRenderTargetView;
		IDXGIFactory* pDXGIFactory1;
		IDXGIAdapter* pDXGIAdapter;
		IDXGIDevice* pDXGIDevice;
		//ID3D11InputLayout* pInputLayout;

	};
}

#endif // ENGINE_RENDERER_DIRECTX_DX11_H

