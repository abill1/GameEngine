#ifndef UNICODE
#define UNICODE
#endif

//================================================================================
//----- Includes
//================================================================================

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <d2d1.h>
#include <windowsx.h>
#include <cstdio>
#include <functional>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXColors.h>
#include <wrl/client.h>

//================================================================================
//----- Macros
//================================================================================

#define CHECK_NULL(x) assert(x!=nullptr)
#define CHECK_EQUALS(x,y) assert(x==y)
#define CHECK_NOT(x,y) assert(x!=y)
#define UNUSED_VAR(x) (void(x))
#define COMPTR(T, x) Microsoft::WRL::ComPtr<T> x;

//================================================================================
//----- Structures
//================================================================================

struct Mouse
{
	int x;
	int y;
	unsigned __int8 buttons;
};

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

// TODO: Complete the list of possible events
enum class MOUSE_INPUTS : unsigned __int8
{
	LEFT_DOWN,
	LEFT_UP,
	RIGHT_DOWN,
	RIGHT_UP,
	MOVE,
	WHEEL
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
// ----- Input devices
const int KEYS = 256;
const int NUM_MOUSE_INPUTS = 256; // TODO: update with the exact number of mouse inputs
std::function<void()> Inputs[KEYS];
std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> MouseInputs[256];
Mouse mouse;

//================================================================================
//----- Function Declarations
//================================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
D2D1_POINT_2F PixelsToDips(HWND _hwnd, float _x, float _y);
void ConsoleLog(const wchar_t* Text, ...);
void ConsoleErr(const wchar_t* Text, ...);
void ConsoleWrn(const wchar_t* Text, ...);
void StubKeyFunc();
void StubMouseFunc(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnLeftDown(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnLeftUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnMove(HWND hwnd, WPARAM wParam, LPARAM lParam);
void OnExit();
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
	AllocConsole();
	ConsoleErr(L"Hello World\n");
	ConsoleWrn(L"It was nice to meet you. I hope to see you again!\n");
	float price = 0.99f;
	ConsoleLog(L"I would like to buy %d %ls from you today $%.2f.\n", 5, L"apples", price);

	// ----- Define user input functions
	using namespace std::placeholders;
	for (int i = 0; i < KEYS; i++)
	{
		Inputs[i] = std::bind(&StubKeyFunc);
		MouseInputs[i] = std::bind(&StubMouseFunc, _1, _2, _3);
	}

	Dog spot;
	Cat felix;

	Inputs['A'] = std::bind(&Dog::Bark, &spot);
	Inputs['W'] = std::bind(&Cat::Meow, &felix);
	Inputs[VK_ESCAPE] = std::bind(&OnExit);

	MouseInputs[(int)MOUSE_INPUTS::LEFT_DOWN] = std::bind(&OnLeftDown, _1, _2, _3);
	MouseInputs[(int)MOUSE_INPUTS::LEFT_UP] = std::bind(&OnLeftUp, _1, _2, _3);
	MouseInputs[(int)MOUSE_INPUTS::MOVE] = std::bind(&OnMove, _1, _2, _3);

	// ----- Register the window class.
	
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);

	// ----- Create the window.
	const wchar_t WINDOW_TITLE[] = L"Game Engine";
	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		WINDOW_TITLE,					// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	CHECK_NULL(hwnd);
	if (hwnd != NULL)
	{
		ShowWindow(hwnd, nCmdShow);
	}

	// ----- DirectX 11 Setup

	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	IDXGISwapChain* pSwapChain;
	ID3D11RenderTargetView* pRenderTargetView;
	IDXGIFactory* pDXGIFactory1;
	IDXGIAdapter* pDXGIAdapter;
	IDXGIDevice* pDXGIDevice;

	// COMPTR(ID3D11Device, pDevice);
	// COMPTR(ID3D11DeviceContext, pDeviceContext);
	// COMPTR(IDXGISwapChain, pSwapChain);
	// COMPTR(ID3D11RenderTargetView, pRenderTargetView);
	// COMPTR(IDXGIFactory, pDXGIFactory1);
	// COMPTR(IDXGIAdapter, pDXGIAdapter);
	// COMPTR(IDXGIDevice, pDXGIDevice);
	
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
	
	RECT rc;
	GetClientRect(hwnd, &rc);
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
	mSwapChainDesc.OutputWindow = hwnd;
	mSwapChainDesc.Windowed = true;
	mSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	mSwapChainDesc.Flags = 0;

	pDXGIFactory1->CreateSwapChain((ID3D11Device*&)pDevice, &mSwapChainDesc, &pSwapChain);
	pDXGIFactory1->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	pDXGIFactory1->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	if (FAILED(hr))
		return hr;

	hr = pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return hr;

	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDeviceContext->RSSetViewports(1, &vp);

	// ---- Game loop
	while (bRunning)
	{
		// ----- Message loop.
		MSG msg = { };
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pDeviceContext->ClearRenderTargetView((ID3D11RenderTargetView*&)pRenderTargetView, DirectX::Colors::MidnightBlue);
		pSwapChain->Present(0, 0);
		
	}

	pDeviceContext->Release();
	pRenderTargetView->Release();
	pSwapChain->Release();
	pDevice->Release();

	FreeConsole();

	return 0;
}

//================================================================================
//----- Functions Defines
//================================================================================

void OnLeftDown(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	SetCapture(hwnd);
	ptMouse = PixelsToDips(hwnd, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
	ellipse.point = ptMouse;
	ellipse.radiusX = 1.0f;
	ellipse.radiusY = 1.0f;
	ConsoleLog(L"Mouse Point: (X:{%.2f},Y:{%.2f})\n", ptMouse.x, ptMouse.y);
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

void OnExit()
{
	//D2RenderTestClose();
	// ----- Update Game state
	bRunning = false;
	// ----- Close window
	PostQuitMessage(0);
}

void Cat::Meow()
{
	x += 2;
	y++;
	ConsoleLog(L"Felix Meows in direction x:%d, y:%d\n", x, y);
}

void Dog::Bark()
{
	x += 2;
	y++;
	ConsoleLog(L"Spot Barks in direction x:%d, y:%d\n", x, y);
}

void StubKeyFunc()
{
	ConsoleLog(L"No user defined input assigned.\n");
}

void StubMouseFunc(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UNUSED_VAR(hwnd);
	UNUSED_VAR(wParam);
	UNUSED_VAR(lParam);
}

void ConsoleLog(const wchar_t* Text, ...)
{
#if _DEBUG
	const int MAX_BUF_SIZE = 4096;
	static wchar_t WIDE_CONSOLE_BUF[MAX_BUF_SIZE];
	memset(WIDE_CONSOLE_BUF, 0, MAX_BUF_SIZE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	DWORD cwr = {};
	
	// ----- Read in any variable arguments
	va_list args;
	va_start(args, Text);
	vswprintf_s((LPWSTR)WIDE_CONSOLE_BUF, MAX_BUF_SIZE, (LPCWSTR)Text, args); // TODO: Figure out issue with float args not being passed. 
	va_end(args);

	// ----- Write to console window
	WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

	// ----- Write to Visual Studio Output
	OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
}

void ConsoleErr(const wchar_t* Text, ...)
{
#if _DEBUG
	const int MAX_BUF_SIZE = 4096;
	static wchar_t WIDE_CONSOLE_BUF[MAX_BUF_SIZE];
	memset(WIDE_CONSOLE_BUF, 0, MAX_BUF_SIZE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
	DWORD cwr = {};

	// ----- Read in any variable arguments
	va_list args;
	va_start(args, Text);
	vswprintf_s((LPWSTR)WIDE_CONSOLE_BUF, MAX_BUF_SIZE, (LPCWSTR)Text, args); // TODO: Figure out issue with float args not being passed. 
	va_end(args);

	// ----- Write to console window
	WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

	// ----- Write to Visual Studio Output
	OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
}

void ConsoleWrn(const wchar_t* Text, ...)
{
#if _DEBUG
	const int MAX_BUF_SIZE = 4096;
	static wchar_t WIDE_CONSOLE_BUF[MAX_BUF_SIZE];
	memset(WIDE_CONSOLE_BUF, 0, MAX_BUF_SIZE);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	DWORD cwr = {};

	// ----- Read in any variable arguments
	va_list args;
	va_start(args, Text);
	vswprintf_s((LPWSTR)WIDE_CONSOLE_BUF, MAX_BUF_SIZE, (LPCWSTR)Text, args); // TODO: Figure out issue with float args not being passed. 
	va_end(args);

	// ----- Write to console window
	WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

	// ----- Write to Visual Studio Output
	OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool InFocus = true;
	PAINTSTRUCT ps;
	HDC hdc;
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		//result = D2RenderTestInit();
		break;
	}
	case WM_QUIT:
	case WM_DESTROY:
	{
		OnExit();
		break;
	}
	case WM_KILLFOCUS:
	{
		InFocus = false;
		mouse.buttons = 0;
		break;
	}
	case WM_SETFOCUS:
	{
		InFocus = true;
		break;
	}
	case WM_SYSKEYUP:
	case WM_KEYUP:
	{
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		if (InFocus)
		{
			WORD keyFlags = HIWORD(lParam);
			WORD scanCode = LOBYTE(keyFlags);
			bool isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
			if (isExtendedKey)
				scanCode = MAKEWORD(scanCode, 0xE0);
			bool wasDown = ((lParam & KF_REPEAT) != 0);
			bool isDown = ((lParam & KF_UP) == 0);
			if (wasDown != isDown)
			{
				Inputs[wParam]();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		MouseInputs[(int)MOUSE_INPUTS::LEFT_DOWN](hwnd, wParam, lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		MouseInputs[(int)MOUSE_INPUTS::LEFT_UP](hwnd, wParam, lParam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		MouseInputs[(int)MOUSE_INPUTS::MOVE](hwnd, wParam, lParam);
		break;
	}
	case WM_PAINT:
	{
		//D2DRenderTest(hwnd);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_SIZE:
	{
		//D2DRenderTestResize(hwnd);
		break;
	}
	default:
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return result;
}

