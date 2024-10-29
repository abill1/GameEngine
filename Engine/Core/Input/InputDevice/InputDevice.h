
#ifndef ENGINE_CORE_INPUT_INPUTDEVICE_H
#define ENGINE_CORE_INPUT_INPUTDEVICE_H

#include "../Keyboard/Keyboard.h"
#include "../Mouse/Mouse.h"
#include <windows.h>

namespace Engine
{
	class InputDevice
	{
	public:
		InputDevice();
		~InputDevice() = default;

		void BindMouseInput(MOUSE_INPUTS _input, std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> _func);
		void BindKeyboardInput(char _key, std::function<void()> _func);

		void ExecuteMouse(MOUSE_INPUTS _input, HWND _hwnd, WPARAM _wParam, LPARAM _lParam);
		void ExecuteKeyboard(char _key);

	private:
		Keyboard keyboard;
		Mouse mouse;

	};
}

#endif // ENGINE_CORE_INPUT_INPUTDEVICE_H

