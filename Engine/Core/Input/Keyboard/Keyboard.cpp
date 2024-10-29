#include "Keyboard.h"
#include "../../Console/Console.h"

void StubKeyFunc()
{
	Engine::Console::Log(L"STUB FUNCTION: No user defined input assigned.\n");
}

Engine::Keyboard::Keyboard()
{
	for (int i = 0; i < NUM_KEY_INPUTS; i++)
	{
		inputs[i] = std::bind(&StubKeyFunc);
	}
}

void Engine::Keyboard::Assign(char _key, std::function<void()> _func)
{
	inputs[(int)_key] = std::bind(_func);
}

void Engine::Keyboard::Execute(char _key)
{
	inputs[(int)_key]();
}

