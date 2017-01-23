#pragma once

#include <Windows.h>
#include <map>

class Window
{
public:

	//constructors/destructors
	Window(int width, int height, char const * title);
	~Window();

	//public methods
	void MakeCurrent() const;
	void SwapBuffers() const;

private:

	//static data
	static char const * const		s_windowClassName;
	static WNDCLASSEX				s_windowClass;
	static std::map<HWND, Window*>	s_windows;

	//static wndproc router
	static LRESULT CALLBACK WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//private methods
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//managed objects
	HWND		m_handle;
	HDC			m_device;
	HGLRC		m_context;

};