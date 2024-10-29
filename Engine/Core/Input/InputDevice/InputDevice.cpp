#include "InputDevice.h"

Engine::InputDevice::InputDevice()
	:keyboard(), mouse()
{

}

void Engine::InputDevice::BindMouseInput(MOUSE_INPUTS _input, std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> _func)
{
	mouse.Assign(_input, _func);
}

void Engine::InputDevice::BindKeyboardInput(char _key, std::function<void()> _func)
{
	keyboard.Assign(_key, _func);
}

void Engine::InputDevice::ExecuteMouse(MOUSE_INPUTS _input, HWND _hwnd, WPARAM _wParam, LPARAM _lParam)
{
	mouse.Execute(_input, _hwnd, _wParam, _lParam);
}

void Engine::InputDevice::ExecuteKeyboard(char _key)
{
	keyboard.Execute(_key);
}


