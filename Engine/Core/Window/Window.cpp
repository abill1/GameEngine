
#include "Window.h"

Engine::Window::Window(wchar_t* _title, int _width, int _height)
	:inputDevice(), mHwnd(), width(_width), height(_height), title(_title), bOpen(false)
{

}

void Engine::Window::Create(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASS wc = { };
	wc.lpfnWndProc = &Window::WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = Window::CLASS_NAME;
	RegisterClass(&wc);
	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT wr = { 0 };
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, style, FALSE);

	// ----- Create the window.
	this->mHwnd = CreateWindowEx(
		0,                              // Optional window styles.
		Window::CLASS_NAME,             // Window class
		title.c_str(),					// Window text
		WS_OVERLAPPEDWINDOW,            // Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		this        // Additional application data
	);

	SetWindowLongPtr(mHwnd, GWLP_USERDATA, (LONG_PTR)this);

	if (mHwnd != NULL)
	{
		ShowWindow(mHwnd, nCmdShow);
		bOpen = true;
	}
}

LRESULT Engine::Window::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool InFocus = true;
	PAINTSTRUCT ps;
	HDC hdc;
	LRESULT result = 0;
	switch (message)
	{
	case WM_QUIT:
	case WM_DESTROY:
	{
		OnExit();
		break;
	}
	case WM_KILLFOCUS:
	{
		InFocus = false;
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
				inputDevice.ExecuteKeyboard((char)wParam);
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		inputDevice.ExecuteMouse(Engine::MOUSE_INPUTS::LEFT_DOWN, mHwnd, wParam, lParam);
		break;
	}
	case WM_LBUTTONUP:
	{
		inputDevice.ExecuteMouse(Engine::MOUSE_INPUTS::LEFT_UP, mHwnd, wParam, lParam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		inputDevice.ExecuteMouse(Engine::MOUSE_INPUTS::MOVE, mHwnd, wParam, lParam);
		break;
	}
	case WM_PAINT:
	{
		break;
	}
	case WM_SIZE:
	{
		break;
	}
	default:
		result = DefWindowProc(mHwnd, message, wParam, lParam);
		break;
	}
	return result;
}

void Engine::Window::SetHwnd(HWND _hwnd)
{
	mHwnd = _hwnd;
}

LRESULT CALLBACK Engine::Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Window* pThis = nullptr;
	LRESULT result = 0;

	if (message == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (Engine::Window*)pCreate->lpCreateParams;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		((Engine::Window*)pThis)->SetHwnd(hwnd);
	}
	else
	{
		pThis = (Engine::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}
	
	if (pThis)
	{
		result = ((Engine::Window*)pThis)->ProcessMessage(message, wParam, lParam);
	}

	return result;
}

void Engine::Window::OnExit()
{
	// ----- Close window
	bOpen = false;
	PostQuitMessage(0);
}
















