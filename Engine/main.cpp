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

//================================================================================
//----- Function Declarations
//================================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//================================================================================
//----- Main
//================================================================================

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
	UNUSED_VAR(hPrevInstance);
	UNUSED_VAR(pCmdLine);

	// ----- Register the window class.
	const wchar_t CLASS_NAME[] = L"WindowClass";
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

	return 0;
}

//================================================================================
//----- Functions Defines
//================================================================================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &p2dFactory)))
			result =  -1;
		break;// return 0;
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
		break;// return 0;
	}
	case WM_PAINT:
	{
		long hr = S_OK;
		if (p2dRenderTarget == nullptr)
		{
			RECT rc = {};
			GetClientRect(hwnd, &rc);
			D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

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

		//PAINTSTRUCT ps;
		//HDC hdc = BeginPaint(hwnd, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.

		//FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		//EndPaint(hwnd, &ps);
		break;// return 0;
	}
	case WM_SIZE:
	{
		if (p2dRenderTarget != NULL)
		{
			RECT rc;
			GetClientRect(hwnd, &rc);

			D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

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
		break;// return 0;
	}
	default:
		result = DefWindowProc(hwnd, uMsg, wParam, lParam);

	}
	return result;
}

