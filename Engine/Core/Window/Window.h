
#ifndef ENGINE_CORE_WINDOW_H
#define ENGINE_CORE_WINDOW_H

#include <windows.h>
#include "../Input/InputDevice/InputDevice.h"

namespace Engine
{
	class InputDevice;

	class Window
	{
	public:
		Window()=delete;
		Window(wchar_t* _title, int _width, int _height);
		~Window() = default;

		void Create(HINSTANCE hInstance, int nCmdShow);
		LRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

		inline InputDevice* GetInputDevice() { return &inputDevice; }
		inline HWND GetHwnd() { return mHwnd; }
		inline int GetWidth() { return width; }
		inline int GetHeight() { return height; }
		inline bool IsOpen() { return bOpen; }

		void SetHwnd(HWND _hwnd);
		void OnExit();

	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static constexpr wchar_t* CLASS_NAME = L"WindowClass";
		
	private:
		InputDevice inputDevice;
		std::wstring title;
		HWND mHwnd;
		int width;
		int height;
		bool bOpen;
		

		
	};
}

#endif // ENGINE_CORE_WINDOW_H

