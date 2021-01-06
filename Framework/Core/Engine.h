#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <Windows.h>
#include "Framework/Core/Subsystem/Scene/MMD2FBX_Converter.h"

namespace Framework
{
	class Engine final
	{
	public:
		Engine();
		~Engine();

		void Init(unsigned int screenWidth, unsigned int screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear);
		void Update();

		void Set_ModelPath(const std::wstring& path) { _model_path = path; }
		void Set_AnimPath(const std::wstring& path);
		void Set_AnimFolder(const std::wstring& path);
		void Set_Run(bool var) { _is_run = var; }
		bool Is_Run() { return _is_run; }

		class Context* GetContext() { return _context.get(); }
		class MMD2FBX_Converter* GetConverter() { return _converter; }

	private:
		std::unique_ptr<class Context> _context;
		class Timer* _timer;
		class MMD2FBX_Converter* _converter;

		std::wstring _model_path;
		std::queue<std::wstring> _anim_paths;
		bool _is_run = false;
	};
}