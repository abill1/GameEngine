
#ifndef ENGINE_CORE_INPUT_KEYBOARD_H
#define ENGINE_CORE_INPUT_KEYBOARD_H

#include <functional>
#include <windows.h>

namespace Engine
{
	class Keyboard
	{
	public:

		Keyboard();
		~Keyboard() = default;

		void Assign(char _key, std::function<void()> _func);
		void Execute(char _key);

	private:
		static constexpr int NUM_KEY_INPUTS = 256;
		std::function<void()> inputs[NUM_KEY_INPUTS];

	};
}

#endif // ENGINE_CORE_INPUT_KEYBOARD_H

