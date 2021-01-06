#pragma once
#include <functional>
#include <Windows.h>
#include <assert.h>

namespace Window
{
	std::function<void(HDC)> Paint;
	std::function<void(WPARAM)> Command;
	std::function<void(WPARAM)> Typing;

	HINSTANCE hInstance;
	HWND hWnd;

	HBITMAP backBuffer;
	HBITMAP oldBuffer;

	inline LRESULT CALLBACK WndProc
	(
		HWND handle,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		static char str[256];
		int len;

		switch (message)
		{
		case WM_COMMAND:
		{
			if (Command != nullptr)
				Command(wParam);
			break;
		}
		case WM_CHAR:
		{
			if (Typing != nullptr)
				Typing(wParam);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HDC hMemDc = CreateCompatibleDC(hdc);;
			RECT rect;
			char Str[32]; // 그냥 문자열 선언

			GetClientRect(hWnd, &rect); //영역을 잡아준다.

			backBuffer = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
			oldBuffer = (HBITMAP)SelectObject(hMemDc, backBuffer);
			//BitBlt(hMemDc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, 0, 0, SRCCOPY);
			PatBlt(hMemDc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, WHITENESS);
			if (Paint != nullptr)
				Paint(hMemDc);

			BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDc, 0, 0, SRCCOPY);
			DeleteObject(SelectObject(hMemDc, oldBuffer));
			DeleteDC(hMemDc);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(handle, message, wParam, lParam);
		}

		return 0;
	}

	inline void Create
	(
		HINSTANCE instance,
		const UINT& width,
		const UINT& height
	)
	{
		WNDCLASSEX wndClass;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hIcon = LoadIcon(nullptr, IDI_ERROR);
		wndClass.hIconSm = LoadIcon(nullptr, IDI_ERROR);
		wndClass.hInstance = instance;
		wndClass.lpfnWndProc = reinterpret_cast<WNDPROC>(WndProc); // static_cast<WNDPROC>(WndProc);
		wndClass.lpszClassName = L"KeyFrameMaker";
		wndClass.lpszMenuName = nullptr;
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.cbSize = sizeof(WNDCLASSEX);

		WORD check = RegisterClassEx(&wndClass);
		assert(check != 0);

		hInstance = instance;

		hWnd = CreateWindowExW
		(
			WS_EX_APPWINDOW,
			L"KeyFrameMaker",
			L"KeyFrameMaker",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<int>(width),
			static_cast<int>(height),
			nullptr,
			nullptr,
			instance,
			nullptr
		);
		assert(hWnd != nullptr);
	}

	inline void Show()
	{
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
		ShowCursor(TRUE);
		ShowWindow(hWnd, SW_SHOWNORMAL);
		UpdateWindow(hWnd);
	}

	inline const bool Update()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	inline void Destroy()
	{
		DeleteObject(backBuffer);
		DestroyWindow(hWnd);
		UnregisterClass(L"KeyFrameMaker", hInstance);
	}

	inline const UINT GetWidth()
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return static_cast<UINT>(rect.right - rect.left);
	}

	inline const UINT GetHeight()
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return static_cast<UINT>(rect.bottom - rect.top);
	}
};