#include "Engine.h"
#include "Util/FileSystem.h"
#include "Core/Engine.h"
#include "Settings.h"
#include <commctrl.h>


using Framework::FileSystem;

LEngine::LEngine() : hWnd(0), hInstance(0), input("")
{
	_f_engine = new Framework::Engine();

}

LEngine::~LEngine()
{
	delete _f_engine;
	_f_engine = nullptr;
}

void LEngine::Initialize()
{
	_f_engine->Init(1, 1, 1, nullptr, false, 1, 1);
	_converter = _f_engine->GetConverter();

	hWnd = Setting::GetHWND();
	hInstance = Setting::Get().GetHInstance();

	input = "";

	Create();
}

void LEngine::Update()
{	
	SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessageW(progress, PBM_SETPOS, _converter->Get_Percentage() * 100, 0);
	
	_f_engine->Update();
}

void LEngine::Create()
{
	CreateWindowW(TEXT("button"), TEXT("Select MMD Model"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 20, 140, 25, hWnd, (HMENU)0, hInstance, NULL);
	CreateWindowW(TEXT("button"), TEXT("Select Vmd Model"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 50, 140, 25, hWnd, (HMENU)1, hInstance, NULL);
	CreateWindowW(TEXT("button"), TEXT("Select Vmd Folder"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 80, 140, 25, hWnd, (HMENU)2, hInstance, NULL);
	CreateWindowW(TEXT("button"), TEXT("Export"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 450, 50, 100, 25, hWnd, (HMENU)3, hInstance, NULL);
	CreateWindowW(TEXT("button"), TEXT("B_Export"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 450, 80, 100, 25, hWnd, (HMENU)4, hInstance, NULL);

	progress = CreateWindowW(PROGRESS_CLASSW, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 180, 550, 10, hWnd, (HMENU)5, hInstance, NULL);
	SendMessageW(progress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	SendMessageW(progress, PBM_SETPOS, 0.f, 0);
	InvalidateRect(hWnd, NULL, FALSE);

	pmx_file_path = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("Set MMD File"), WS_CHILD | WS_VISIBLE | ES_READONLY, 165, 20, 270, 20, hWnd, (HMENU)10, NULL, NULL);
	vmd_file_path = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("Set MMD File"), WS_CHILD | WS_VISIBLE | ES_READONLY, 165, 50, 270, 20, hWnd, (HMENU)11, NULL, NULL);
	vmd_folder_path = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("Set Folder for Converting"), WS_CHILD | WS_VISIBLE | ES_READONLY, 165, 80, 270, 20, hWnd, (HMENU)12, NULL, NULL);

	CreateWindowW(TEXT("static"), TEXT("Tick per Ms"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 10, 120, 90, 20, hWnd, (HMENU)20, hInstance, NULL);
	tick_per_ms = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("30"), WS_CHILD | WS_VISIBLE, 100, 120, 40, 20, hWnd, (HMENU)21, NULL, NULL);
	CreateWindowW(TEXT("static"), TEXT("Start Frame"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 160, 120, 90, 20, hWnd, (HMENU)20, hInstance, NULL);
	frame_start = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE, 250, 120, 40, 20, hWnd, (HMENU)22, NULL, NULL);
	CreateWindowW(TEXT("static"), TEXT("End Frame"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 300, 120, 90, 20, hWnd, (HMENU)20, hInstance, NULL);
	frame_end = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("-1"), WS_CHILD | WS_VISIBLE, 330, 120, 40, 20, hWnd, (HMENU)23, NULL, NULL);
	use_physics = CreateWindowW(TEXT("button"), TEXT("Use Physics"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 440, 120, 100, 20, hWnd, (HMENU)24, hInstance, NULL);
	use_jacobian = CreateWindowW(TEXT("button"), TEXT("Use Simple Jacobian"), WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 385, 150, 160, 20, hWnd, (HMENU)25, hInstance, NULL);
}

void LEngine::Command(WPARAM wParam)
{
	switch (LOWORD(wParam))
	{
	case 0:
	{
		FileSystem::OpenFileDialogW(
			[this](const std::wstring& path) {
				auto relative_path = FileSystem::GetRelativeFilePath(path);
				auto path_part = relative_path.substr(relative_path.size() - 33 < 0 ? 0 : relative_path.size() - 33, relative_path.size() - 1);
				_f_engine->Set_ModelPath(FileSystem::GetSlashPath(relative_path));
				SendMessageW(pmx_file_path, WM_SETTEXT, 0, (LPARAM)(path_part.c_str()));
			},
			L"Supported Files(*.pmx)\0*.pmx", (FileSystem::GetWorkingDirectoryW() + L"../").c_str(), hWnd);
		break;
	}
	case 1:
		FileSystem::OpenFileDialogW(
			[this](const std::wstring& path) {
				auto relative_path = FileSystem::GetRelativeFilePath(path);
				auto path_part = relative_path.substr(relative_path.size() < 33 ? 0 : relative_path.size() - 33, relative_path.size() - 1);
				_f_engine->Set_AnimPath(FileSystem::GetSlashPath(relative_path));
				SendMessageW(vmd_file_path, WM_SETTEXT, 0, (LPARAM)(path_part.c_str()));
				SendMessageW(vmd_folder_path, WM_SETTEXT, 0, (LPARAM)(L""));
				SendMessageW(tick_per_ms, WM_SETTEXT, 0, (LPARAM)(L"30"));
				SendMessageW(frame_start, WM_SETTEXT, 0, (LPARAM)(L"0"));
				SendMessageW(frame_end, WM_SETTEXT, 0, (LPARAM)(L"-1"));
			},
			L"Supported Files(*.vmd)\0*.vmd", (FileSystem::GetWorkingDirectoryW() + L"../").c_str(), hWnd);
		break;
	case 2:
		FileSystem::OpenFilePathDialogW(
			[this](const std::wstring& path) {
				auto relative_path = FileSystem::GetRelativeFilePath(path);
				auto path_part = relative_path.substr(relative_path.size() < 33 ? 0 : relative_path.size() - 33, relative_path.size() - 1);
				_f_engine->Set_AnimFolder(relative_path);
				SendMessageW(vmd_folder_path, WM_SETTEXT, 0, (LPARAM)(path_part.c_str()));
				SendMessageW(vmd_file_path, WM_SETTEXT, 0, (LPARAM)(L""));
				SendMessageW(tick_per_ms, WM_SETTEXT, 0, (LPARAM)(L"30"));
				SendMessageW(frame_start, WM_SETTEXT, 0, (LPARAM)(L"0"));
				SendMessageW(frame_end, WM_SETTEXT, 0, (LPARAM)(L"-1"));
			},
			L"Folder", (FileSystem::GetWorkingDirectoryW() + L"../").c_str(), hWnd);
		break;
	case 3:
	{
		TCHAR buff[1024];
		GetWindowTextW(tick_per_ms, buff, 1024);
		auto f = wcstod(buff, nullptr);
		_converter->Set_Ms_per_Tick(1000.f/f);
		_converter->Set_StartFrame(GetDlgItemInt(hWnd, GetDlgCtrlID(frame_start), nullptr, true));
		_converter->Set_EndFrame(GetDlgItemInt(hWnd, GetDlgCtrlID(frame_end), nullptr, true));
		_converter->Set_Is_For_Binary(false);
		_f_engine->Set_Run(true);
		_is_exporting = true;
		break;
	}
	case 4:
		_converter->Set_StartFrame(GetDlgItemInt(hWnd, GetDlgCtrlID(frame_start), nullptr, true));
		_converter->Set_EndFrame(GetDlgItemInt(hWnd, GetDlgCtrlID(frame_end), nullptr, true));
		_converter->Set_Is_For_Binary(true);
		_f_engine->Set_Run(true);
		_is_exporting = true;
		break;
	case 21:
		if (_f_engine->Is_Run())
			SendMessageW(tick_per_ms, WM_SETTEXT, 0, (LPARAM)(L"30"));
		break;
	case 22:
		if (_f_engine->Is_Run())
			SendMessageW(frame_start, WM_SETTEXT, 0, (LPARAM)(L"0"));
		break;
	case 23:
		if (_f_engine->Is_Run())
			SendMessageW(frame_end, WM_SETTEXT, 0, (LPARAM)(L"-1"));
		break;
	case 24:
		if (_converter->Use_Physics())
		{
			_converter->Set_Use_Physics(false);
			SendMessageW(use_physics, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		else {
			_converter->Set_Use_Physics(true);
			SendMessageW(use_physics, BM_SETCHECK, BST_CHECKED, 0);
		}
		break;
	case 25:
		if (_converter->Use_Jacobian())
		{
			_converter->Set_Use_Jacobian(false);
			SendMessageW(use_jacobian, BM_SETCHECK, BST_UNCHECKED, 0);
		}
		else {
			_converter->Set_Use_Jacobian(true);
			SendMessageW(use_jacobian, BM_SETCHECK, BST_CHECKED, 0);
		}
		break;
	}
	InvalidateRect(hWnd, nullptr, false);
}

void LEngine::Paint(HDC hdc)
{
	TextOutA(hdc, 100, 100, input.c_str(), input.length());
}

void LEngine::Typing(WPARAM wParam)
{
	input += (TCHAR)wParam;
	InvalidateRect(hWnd, NULL, FALSE);
}