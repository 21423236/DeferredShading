#pragma once

#include <Framework/Window.h>
#include <Framework/Scene.h>
#include <Framework/IRenderer.h>
#include <Framework/GUI.h>
#include <Framework/Input.h>

#include <Windows.h>

#include <map>

class Application
{
public:

	friend class Input;

	typedef enum MouseButton {
		LEFT_MOUSE_BUTTON,
		RIGHT_MOUSE_BUTTON,
		MIDDLE_MOUSE_BUTTON
	} MouseButton;

	//constructors/destructor
	Application(IRenderer * renderer);
	~Application();

	//public methods
	void RenderFrame() const;
	int Run();

	std::string OpenFile(char const * filter);

	float dt() const;

protected:

	//events
	virtual void Initialize();
	virtual void MouseClicked(MouseButton button, int x, int y);
	virtual void MouseDragged(MouseButton button, int dx, int dy);
	virtual void MouseScrolled(int delta);

private:

	//static functions/data
	static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static std::map<HWND, Application*> s_applicationDictionary;

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Window * m_window;
	Scene * m_scene;
	IRenderer * m_renderer;
	GUI * m_gui;
	Input * m_input;

	bool m_running;
	bool m_isPaused;

	int m_width;
	int m_height;

};