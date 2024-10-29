
#ifndef ENGINE_CORE_INPUT_MOUSE_H
#define ENGINE_CORE_INPUT_MOUSE_H

#include <functional>
#include <windows.h>

namespace Engine
{
	enum class MOUSE_INPUTS : unsigned __int8
	{
		LEFT_DOWN,
		LEFT_UP,
		RIGHT_DOWN,
		RIGHT_UP,
		MOVE,
		WHEEL
	};

	class Mouse
	{
	public:
		Mouse();
		~Mouse() = default;

		void Assign(MOUSE_INPUTS _input, std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> _func);
		void Execute(MOUSE_INPUTS _input, HWND _hwnd, WPARAM _wParam, LPARAM _lParam);

	private:
		static constexpr int NUM_OF_MOUSE_INPUTS = 6;
		std::function<void(HWND hwnd, WPARAM wParam, LPARAM lParam)> inputs[NUM_OF_MOUSE_INPUTS];

	};
}

#endif // ENGINE_CORE_INPUT_MOUSE_H

