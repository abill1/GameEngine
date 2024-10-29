#include "Mouse.h"
#include "Core/Console/Console.h"

void StubMouseFunc(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	Engine::Console::Log(L"STUB FUNCTION: input not assigned.\n");
}

Engine::Mouse::Mouse()
{
	for (unsigned __int8 i = 0u; i < 6; i++)
	{
		inputs[(unsigned __int8)Engine::MOUSE_INPUTS::LEFT_DOWN + i] = std::bind(&StubMouseFunc, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) ;
	}
}

void Engine::Mouse::Assign(Engine::MOUSE_INPUTS _input, std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> _func)
{
	inputs[(int)_input] = std::bind(_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

void Engine::Mouse::Execute(Engine::MOUSE_INPUTS _input, HWND _hwnd, WPARAM _wParam, LPARAM _lParam)
{
	inputs[(int)_input](_hwnd, _wParam, _lParam);
}
