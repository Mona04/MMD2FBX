#pragma comment (lib, "Framework.lib")
#pragma comment (lib, "FreeImage.lib")
#pragma comment (lib, "libfbxsdk.lib")

#include "Engine.h"
#include "Window.h"
#include "Settings.h"

void Initialize();

int APIENTRY WinMain
(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	LPSTR lpszCmdParam,
	int nCmdShow
)
{
	Window::Create(hInstance, 600, 240);
	Window::Show();

	Initialize();

	LEngine engine;
	engine.Initialize();
	
	Window::Command = std::bind(&LEngine::Command, &engine, std::placeholders::_1);
	Window::Paint = std::bind(&LEngine::Paint, &engine, std::placeholders::_1);
	Window::Typing = std::bind(&LEngine::Typing, &engine, std::placeholders::_1);
	
	while (Window::Update())
		engine.Update();	
	
	Window::Destroy();

	return 0;
}

void Initialize()
{
	Setting::Get().SetHInstance(Window::hInstance);
	Setting::Get().SetHWND(Window::hWnd);
	Setting::Get().SetWidth(Window::GetWidth());
	Setting::Get().SetHeight(Window::GetHeight());
}