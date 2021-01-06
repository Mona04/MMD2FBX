#pragma once
#include <Windows.h>

class Setting final
{
public:
	static Setting& Get() { static Setting instance; return instance; }

private:
	Setting() = default;
	~Setting() = default;

public:
	static HINSTANCE GetHInstance() { return Get().hInstance; }
	void SetHInstance(HINSTANCE hInstance) { this->hInstance = hInstance; }

	static HWND GetHWND() { return Get().hHandle; }
	void SetHWND(HWND hHandle) { this->hHandle = hHandle; }

	UINT GetWidth() { return width; }
	void SetWidth(const UINT& width) { this->width = width; }

	UINT GetHeight() { return height; }
	void SetHeight(const UINT& height) { this->height = height; }

private:
	HINSTANCE hInstance;
	HWND hHandle;
	UINT width;
	UINT height;
};