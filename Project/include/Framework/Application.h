#pragma once

#include <Framework/Window.h>
#include <Framework/Scene.h>
#include <Framework/IRenderer.h>

#include <Windows.h>

#include <map>

class Application
{
public:

	//constructors/destructor
	Application(IRenderer * renderer);
	~Application();

	//public methods
	void RenderFrame() const;
	int Run();

	float dt() const;

private:

	//static functions/data
	static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static std::map<HWND, Application*> s_applicationDictionary;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Window * m_window;
	Scene * m_scene;
	IRenderer * m_renderer;

	bool m_running;

	

};