#ifndef UNICODE
#define UNICODE
#endif

//================================================================================
//----- Includes
//================================================================================

#include <windows.h>
#include <tchar.h>
#include <d2d1.h>
#include <windowsx.h>
#include <cstdio>
#include <functional>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <string.h>
#include <dxgi1_2.h>
#include <stdlib.h>

#include "Core/Core.h"
#include "Vendor/stb/stb_image.h"
#include "Vendor/FreeType/freetype.h"
#include "Renderer/DirectX/DX11/DX11.h"

//================================================================================
//----- Structures
//================================================================================

class Dog
{
public:
	Dog() = default;
	int x;
	int y;
	void Bark();
};

class Cat
{
public:
	Cat() = default;
	int x;
	int y;
	void Meow();
};

struct TexCoord
{
	float u;
	float v;
};

struct Vertex
{
	fVect position;
	fVect color;
	TexCoord texcoord;
};

struct TriangleIndex
{
	unsigned int v0;
	unsigned int v1;
	unsigned int v2;

};

struct Image
{
	float x;
	float y;
	float xWidth;
	float yWidth;
};

struct ImageBuffer
{
	float u;
	float v;
private:
	float pad[2];
};

struct InstanceData
{
	DirectX::XMMATRIX mTRS[16];
	DirectX::XMVECTOR mColors[16];
};

//================================================================================
//----- Globals
//================================================================================

bool bRunning = true;
// ----- d2d1 Graphics
ID2D1Factory* p2dFactory;
ID2D1HwndRenderTarget* p2dRenderTarget;
ID2D1SolidColorBrush* pBrush;
D2D1_ELLIPSE ellipse;
D2D1_POINT_2F ptMouse;
HANDLE hStdout;
DWORD cWritten;

//================================================================================
//----- Function Declarations
//================================================================================

inline D2D1_POINT_2F PixelsToDips(HWND _hwnd, float _x, float _y);
void OnLeftDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnLeftUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
void D2DRenderTest(HWND _hwnd);
void D2DRenderTestResize(HWND _hwnd);
LRESULT D2RenderTestInit();
void D2RenderTestClose();

//================================================================================
//----- Main
//================================================================================

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	UNUSED_VAR(hPrevInstance);
 	UNUSED_VAR(pCmdLine);
 	const wchar_t CLASS_NAME[] = L"WindowClass";
 
 	// ----- Create a Console for Window App
	Engine::Console::Init();
	Engine::Console::Error(L"Hello World\n");
	Engine::Console::Warning(L"It was nice to meet you. I hope to see you again!\n");
 	float price = 0.99f;
	Engine::Console::Log(L"I would like to buy %d %ls from you today $%.2f.\n", 5, L"apples", price);

 	// ----- Define user input functions
 
 	Dog spot;
 	Cat felix;

	// ----- Register Window
	Engine::Window window(L"Game Engine", 1280, 800);
	window.Create(hInstance, SW_SHOW);
 
	// ----- Assign Input actions
	window.GetInputDevice()->BindKeyboardInput('A', std::bind(&Dog::Bark, &spot));
	window.GetInputDevice()->BindKeyboardInput('W', std::bind(&Cat::Meow, &felix));

	window.GetInputDevice()->BindMouseInput(Engine::MOUSE_INPUTS::LEFT_DOWN, &OnLeftDown);
	window.GetInputDevice()->BindMouseInput(Engine::MOUSE_INPUTS::LEFT_UP, &OnLeftUp);
	window.GetInputDevice()->BindMouseInput(Engine::MOUSE_INPUTS::MOVE, &OnMove);
 
	// // ----- Load the FreeType library
	// 
	// FT_Library ft;
	// if (FT_Init_FreeType(&ft))
	// {
	// 	Engine::Console::Error(L"Failed to Load FreeType Library.\n");
	// 	bRunning = false;
	// }
	// 
	// FT_Face face;
	// const char* fontFileLocation = "..\\Assets\\Fonts\\vgafix.fon";
	// if (FT_New_Face(ft, fontFileLocation, 0, &face))
	// {
	// 	Engine::Console::Error(L"Failed to Load font.\n");
	// }
	// 
	// FT_Set_Pixel_Sizes(face, 0, 128);
	// unsigned int glyphIndex =  FT_Get_Char_Index(face, 'A');
	// FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
	// FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	// 
	// //if (FT_Load_Char(face, 'A', FT_LOAD_RENDER))
	// //{
	// //}
	// 
	// FT_GlyphSlot ft_glyph = face->glyph;
	// FT_Bitmap ft_bitmap = ft_glyph->bitmap;
	// 
	// FT_Done_Face(face);
	// FT_Done_FreeType(ft);


 	// ----- DirectX 11 Setup
	Engine::DX11 dx11;
	dx11.CreateRenderTargetView(window.GetHwnd());
 	
 	// Model Data
	float uStart = 216.0f / 384.0f;
	float vStart = 120.0f / 640.0f;
	float uExtent = 24.0f / 384.0f;
	float vExtent = 40.0f / 640.0f;
	
	DirectX::XMMATRIX scaleMat = DirectX::XMMatrixScaling(uExtent, vExtent, 1.0f);
	DirectX::XMMATRIX transMat = DirectX::XMMatrixTranslation(uStart, vStart, 0.0f);
	DirectX::XMMATRIX glyph = scaleMat * transMat;
	
	Vertex vertices[] = {
		{{1.0f, -1.0f, 0.0f, 1.0f },  {0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, 1.0f },
		{{-1.0f, -1.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f, 1.0f}, 0.0f, 1.0f },
		{{-1.0f, 1.0f, 0.0f, 1.0f },  {1.0f, 0.0f, 0.0f, 1.0f}, 0.0f, 0.0f },
		{{1.0f, 1.0f, 0.0f, 1.0f  },  {0.0f, 1.0f, 1.0f, 1.0f}, 1.0f, 0.0f }
	};
	
	TriangleIndex indices[] = {
		{0, 1, 2},
		{2, 3, 0}
	};
	
	// Shader setup
 	std::wstring SHADER_ROOT = L"";
 	if (IsDebuggerPresent())
 	{
 #ifdef _DEBUG
 		SHADER_ROOT = L"..\\build\\bin\\x86_64-Debug\\";
 #elif NDEBUG
 		SHADER_ROOT = L"..\\build\\bin\\x86_64-Debug\\";
 #endif
 	}
	 
	 const wchar_t* VERT_FILE = L"Shader_vs.cso";
	 const wchar_t* PIXEL_FILE = L"Shader_ps.cso";
 	 
 	 DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
 #ifdef _DEBUG
 	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
 	// Setting this flag improves the shader debugging experience, but still allows 
 	// the shaders to be optimized and to run exactly the way they will run in 
 	// the release configuration of this program.
 	dwShaderFlags |= D3DCOMPILE_DEBUG;
 
 	// Disable optimizations to further improve shader debugging
 	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
 #endif
 
 	// Vert Shader
 	ID3DBlob* pErrorBlob = nullptr;
 	std::wstring vertPath = SHADER_ROOT + VERT_FILE;
	ID3D10Blob* vertexShaderBuffer = dx11.LoadShaderData(vertPath.c_str());
	ID3D11VertexShader* pVertexShader = dx11.CreateVertexShader(vertexShaderBuffer);
	ID3D11InputLayout* pInputLayout = dx11.CreateInputLayout(vertexShaderBuffer);

 	// Pixel Shader
 	std::wstring pixelPath = SHADER_ROOT + PIXEL_FILE;
	ID3D10Blob* pixelShaderBuffer = dx11.LoadShaderData(pixelPath.c_str());
	ID3D11PixelShader* pPixelShader = dx11.CreatePixelShader(pixelShaderBuffer);
	
 	// Vertex data
	ID3D11Buffer* pVertexBuffer = dx11.CreateVertexBuffer(sizeof(vertices), vertices);
	
	ID3D11Buffer* pIndexBuffer = dx11.CreateIndexBuffer(sizeof(indices), indices);
	
	ID3D11SamplerState* pSamplerState = dx11.CreateSamplerState();
	
	const char* pFontFile = "F:\\programming_projects\\GameEngine\\Assets\\Textures\\FontSheetFixedsys.tga";
	ID3D11Texture2D* pWallTexture = dx11.CreateTexture(pFontFile);
	ID3D11ShaderResourceView* pTexture = dx11.CreateShaderResourceView(pWallTexture);
	
	ID3D11BlendState* pBlendState = dx11.CreateBlendState();
	ID3D11Buffer* pImgCBuf = dx11.CreateConstantBuffer(sizeof(glyph), &glyph);
	
	float scale = 0.425f;
	InstanceData instData;
	instData.mTRS[0] = DirectX::XMMatrixTranslation(-1.35f, 0.0f, 0.0f) * DirectX::XMMatrixScaling(scale, scale, scale);
	instData.mTRS[1] = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f)  * DirectX::XMMatrixScaling(scale, scale, scale);
	instData.mTRS[2] = DirectX::XMMatrixTranslation(1.35f, 0.0f, 0.0f) * DirectX::XMMatrixScaling(scale, scale, scale);
	instData.mColors[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	instData.mColors[1] = { 1.0f, 1.0f, 0.0f, 1.0f };
	instData.mColors[2] = { 1.0f, 1.0f, 1.0f, 1.0f };
	ID3D11Buffer* pTRSCBuf = dx11.CreateConstantBuffer(sizeof(instData), &instData);
	D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.0f, 0.0f, 1280.0f, 800.0f);
	fVect clearColor = { 0.2f, 0.2f, 0.2f, 1.0f };

 	// ---- Game loop
 	while (window.IsOpen())
 	{
 		// ----- Message loop.
 		MSG msg = { };
 		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0)
 		{
 			TranslateMessage(&msg);
 			DispatchMessage(&msg);
 		}
 
 		//-----------------------------------------------------------------
 		// ----- Rendering
 		//-----------------------------------------------------------------
 
 		// ------ Clear Screen
		dx11.ClearScreen(clearColor);
		dx11.SetViewport(&viewport);
		dx11.SetRenderTarget(nullptr);
		dx11.SetBlendState(pBlendState);

		// ----- Define draw conditions;
		dx11.SetInputLayout(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pInputLayout);
		dx11.SetConstantBuffer(pImgCBuf, 0);
		dx11.SetConstantBuffer(pTRSCBuf, 1);
		dx11.SetVertexShader(pVertexShader);
		dx11.SetPixelShader(pPixelShader);
		dx11.SetSampler(pSamplerState);
		dx11.SetTexture(pTexture, 0);
 		
 		unsigned int stride = sizeof(Vertex);
 		unsigned int offset = 0;
		dx11.SetVertexBuffer(pVertexBuffer, stride, offset, 0);
		dx11.SetIndexBuffer(pIndexBuffer);

 		// ----- Send data to Draw
		dx11.DrawInstanced(6, 3);

 		// ----- Show and swap
		dx11.Present();

 	}
 
	//-----------------------------------------------------------------
	// ----- Clean up
	//-----------------------------------------------------------------

	RELEASECOM(pImgCBuf);
	RELEASECOM(pTRSCBuf);
	RELEASECOM(pWallTexture);
	RELEASECOM(pTexture);
	RELEASECOM(pBlendState);
	RELEASECOM(pSamplerState);
	RELEASECOM(pVertexBuffer);
	RELEASECOM(pIndexBuffer);
	RELEASECOM(pVertexShader);
	RELEASECOM(pPixelShader);
	RELEASECOM(vertexShaderBuffer);
 
	Engine::Console::Release();

	return 0;
}

//================================================================================
// ----- Functions Defines
//================================================================================

void OnLeftDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	SetCapture(hwnd);
	ptMouse = PixelsToDips(hwnd, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
	ellipse.point = ptMouse;
	ellipse.radiusX = 1.0f;
	ellipse.radiusY = 1.0f;
	Engine::Console::Log(L"Mouse Point: (X:{%.2f},Y:{%.2f})\n", ptMouse.x, ptMouse.y);
	InvalidateRect(hwnd, nullptr, FALSE);
}

void OnLeftUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UNUSED_VAR(hwnd);
	UNUSED_VAR(wParam);
	UNUSED_VAR(lParam);
	ReleaseCapture();
}

void OnMove(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	float pixelx = (float)GET_X_LPARAM(lParam);
	float pixely = (float)GET_Y_LPARAM(lParam);
	DWORD flags = (DWORD)wParam;
	if (flags & MK_LBUTTON)
	{
		D2D1_POINT_2F dips = PixelsToDips(hwnd, pixelx, pixely);
		float width = (dips.x - ptMouse.x) / 2.0f;
		float height = (dips.y - ptMouse.y) / 2.0f;
		float x1 = ptMouse.x + width;
		float y1 = ptMouse.y + height;
		ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		InvalidateRect(hwnd, nullptr, FALSE);
	}
}

void Cat::Meow()
{
	x += 2;
	y++;
	Engine::Console::Log(L"Felix Meows in direction x:%d, y:%d\n", x, y);
}

void Dog::Bark()
{
	x += 2;
	y++;
	Engine::Console::Log(L"Spot Barks in direction x:%d, y:%d\n", x, y);
}

D2D1_POINT_2F PixelsToDips(HWND _hwnd, float _x, float _y)
{
	float dpi = (float)GetDpiForWindow(_hwnd);
	float scale = dpi / 96.0f;
	return(D2D1::Point2F(_x / scale, _y / scale));
}

void D2DRenderTest(HWND _hwnd)
{
	long hr = S_OK;
	if (p2dRenderTarget == nullptr)
	{
		RECT rc = {};
		GetClientRect(_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU((UINT32)rc.right, (UINT32)rc.bottom);

		hr = p2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(_hwnd, size), &p2dRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0.0f);
			hr = p2dRenderTarget->CreateSolidColorBrush(color, &pBrush);
			if (SUCCEEDED(hr))
			{
				if (p2dRenderTarget != nullptr)
				{
					D2D1_SIZE_F size = p2dRenderTarget->GetSize();
					const float x = size.width / 2.0f;
					const float y = size.height / 2.0f;
					const float radius = min(x, y);
					ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
				}
			}
		}
	}

	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(_hwnd, &ps);
		p2dRenderTarget->BeginDraw();
		p2dRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		p2dRenderTarget->FillEllipse(ellipse, pBrush);
		hr = p2dRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			CHECK_NULL(p2dRenderTarget);
			p2dRenderTarget->Release();
			CHECK_NULL(pBrush);
			pBrush->Release();
		}
		EndPaint(_hwnd, &ps);
	}
}

void D2DRenderTestResize(HWND _hwnd)
{
	if (p2dRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU((UINT32)rc.right, (UINT32)rc.bottom);

		p2dRenderTarget->Resize(size);
		if (p2dRenderTarget != NULL)
		{
			D2D1_SIZE_F size = p2dRenderTarget->GetSize();
			const float x = size.width / 2.0f;
			const float y = size.height / 2.0f;
			const float radius = min(x, y);
			ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
		}
		InvalidateRect(_hwnd, NULL, FALSE);
	}
}

LRESULT D2RenderTestInit()
{
	LRESULT result = 0;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p2dFactory)))
		result = -1;
	return result;
}

void D2RenderTestClose()
{
	// ----- Release 2d Graphic objects
	CHECK_NULL(p2dRenderTarget);
	p2dRenderTarget->Release();
	CHECK_NULL(pBrush);
	pBrush->Release();
	CHECK_NULL(p2dFactory);
	p2dFactory->Release();
}

