
#ifndef ENGINE_CORE_CONSOLE_H
#define ENGINE_CORE_CONSOLE_H
#include <consoleapi.h>
#include <processenv.h>
#include <consoleapi2.h>
#include <cstdio>
#include <winbase.h>
#include <cstdarg>

namespace Engine
{
	namespace Console
	{
		void Init()
		{
#if _DEBUG
			AllocConsole();
#endif
		}

		void Log(const wchar_t* Text, ...)
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
			WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

			// ----- Write to Visual Studio Output
			OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
		}
		void Error(const wchar_t* Text, ...)
		{
#if _DEBUG
			const int MAX_BUF_SIZE = 4096;
			static wchar_t WIDE_CONSOLE_BUF[MAX_BUF_SIZE];
			memset(WIDE_CONSOLE_BUF, 0, MAX_BUF_SIZE);
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			DWORD cwr = {};

			// ----- Read in any variable arguments
			va_list args;
			va_start(args, Text);
			vswprintf_s((LPWSTR)WIDE_CONSOLE_BUF, MAX_BUF_SIZE, (LPCWSTR)Text, args); // TODO: Figure out issue with float args not being passed. 
			va_end(args);

			// ----- Write to console window
			WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

			// ----- Write to Visual Studio Output
			OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
		}
		void Warning(const wchar_t* Text, ...)
		{
#if _DEBUG
			const int MAX_BUF_SIZE = 4096;
			static wchar_t WIDE_CONSOLE_BUF[MAX_BUF_SIZE];
			memset(WIDE_CONSOLE_BUF, 0, MAX_BUF_SIZE);
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			DWORD cwr = {};

			// ----- Read in any variable arguments
			va_list args;
			va_start(args, Text);
			vswprintf_s((LPWSTR)WIDE_CONSOLE_BUF, MAX_BUF_SIZE, (LPCWSTR)Text, args); // TODO: Figure out issue with float args not being passed. 
			va_end(args);

			// ----- Write to console window
			WriteConsole(hOut, WIDE_CONSOLE_BUF, (DWORD)lstrlen(WIDE_CONSOLE_BUF), &cwr, nullptr);

			// ----- Write to Visual Studio Output
			OutputDebugStringW(WIDE_CONSOLE_BUF);
#endif
		}
	}
}

#endif

