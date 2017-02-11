#pragma once

#include <Windows.h>
#include <map>

class Window
{
public:

	friend class Application;

	//constructors/destructors
	Window(LRESULT(CALLBACK * WndProc)(HWND, UINT, WPARAM, LPARAM));
	~Window();

	//public methods
	void MakeCurrent() const;
	void SwapBuffers() const;
	bool Create(int width, int height, char const * title);
	void Destroy();

	//getters
	HWND const & GetHandle() const;

private:

	//static data
	static char const * const		s_windowClassName;
	static WNDCLASSEX				s_windowClass;
	static bool                     s_windowClassIsRegistered;

	//managed objects
	HWND		m_handle;
	HDC			m_device;
	HGLRC		m_context;
	int			m_width;
	int			m_height;

};