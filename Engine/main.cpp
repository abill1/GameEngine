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

//================================================================================
//----- Macros
//================================================================================

#define CHECK_NULL(x) assert(x!=nullptr)
#define CHECK_EQUALS(x,y) assert(x==y)
#define CHECK_NOT(x,y) assert(x!=y)
#define UNUSED_VAR(x) (void(x))

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
D2D1_POINT_2F PixelsToDips(HWND _hwnd, float _x, float _y);
void ConsoleLog(const wchar_t* Text, ...);

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
	ConsoleLog(L"Hello World\n");
	ConsoleLog(L"It was nice to meet you. I hope to see you again!\n");
	float price = 0.99f;
	ConsoleLog(L"I would like to buy %d %ls from you today $%.2f.", 5, L"apples", price);
	

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
	}

	FreeConsole();

	return 0;
}

//================================================================================
//----- Functions Defines
//================================================================================


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
	WriteConsole(hOut, WIDE_CONSOLE_BUF, lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p2dFactory)))
			result =  -1;
		break;
	}
	case WM_DESTROY:
	{
		// ----- Release 2d Graphic objects
		CHECK_NULL(p2dRenderTarget);
		p2dRenderTarget->Release();
		CHECK_NULL(pBrush);
		pBrush->Release();
		CHECK_NULL(p2dFactory);
		p2dFactory->Release();
		// ----- Update Game state
		bRunning = false;
		// ----- Close window
		PostQuitMessage(0);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetCapture(hwnd);
		ptMouse = PixelsToDips(hwnd, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		ellipse.point = ptMouse;
		ellipse.radiusX = 1.0f;
		ellipse.radiusY = 1.0f;
		printf("Mouse Point: (X:{%.2f},Y:{%.2f})\n ", ptMouse.x, ptMouse.y);
		InvalidateRect(hwnd, nullptr, FALSE);
		break;
	}
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE:
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
		break;
	}
	case WM_PAINT:
	{
		long hr = S_OK;
		if (p2dRenderTarget == nullptr)
		{
			RECT rc = {};
			GetClientRect(hwnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU((UINT32)rc.right, (UINT32)rc.bottom);

			hr = p2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &p2dRenderTarget);
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
			BeginPaint(hwnd, &ps);
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
			EndPaint(hwnd, &ps);
		}
		break;
	}
	case WM_SIZE:
	{
		if (p2dRenderTarget != NULL)
		{
			RECT rc;
			GetClientRect(hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU((UINT32)rc.right, (UINT32)rc.bottom);

			p2dRenderTarget->Resize(size);
			if (p2dRenderTarget != NULL)
			{
				D2D1_SIZE_F size = p2dRenderTarget->GetSize();
				const float x = size.width / 2;
				const float y = size.height / 2;
				const float radius = min(x, y);
				ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);
			}
			InvalidateRect(hwnd, NULL, FALSE);
		}
		break;
	}
	default:
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		break;
	}
	return result;
}

