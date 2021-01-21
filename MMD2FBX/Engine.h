#pragma once
#include <Windows.h>
#include <string>

namespace Framework
{
	class Engine;
	class MMD2FBX_Converter;
};

class LEngine
{
public:
	LEngine();
	~LEngine();

	void Initialize();
	void Update();

	void Create();
	void Command(WPARAM wParam);
	void Typing(WPARAM wParam);
	void Paint(HDC hdc);

private:
	std::string input;

	HWND hWnd;
	HINSTANCE hInstance;

	Framework::MMD2FBX_Converter* _converter = nullptr;
	Framework::Engine* _f_engine = nullptr;
	bool _is_exporting = false;

	HWND progress;
	HWND pmx_file_path;
	HWND vmd_file_path;
	HWND vmd_folder_path;

	HWND tick_per_ms;
	HWND frame_start;
	HWND frame_end;
	HWND use_physics;
	HWND use_jacobian;
};