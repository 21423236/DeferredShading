#include <Framework/Window.h>

#include <cstring>

#pragma region "Static Data"

char const * const  Window::s_windowClassName = "WindowClassName";
WNDCLASSEX          Window::s_windowClass = { 0 };
bool				Window::s_windowClassIsRegistered = false;

#pragma endregion

#pragma region "Constructors/Destructors"

Window::Window(LRESULT (CALLBACK * WndProc)(HWND, UINT, WPARAM, LPARAM)) : m_handle(0), m_device(0), m_context(0), m_width(0), m_height(0)
{
	if (!s_windowClassIsRegistered)
	{
		s_windowClass.cbSize = sizeof(WNDCLASSEX);
		s_windowClass.style = CS_OWNDC;
		s_windowClass.lpfnWndProc = WndProc;
		s_windowClass.cbClsExtra = 0;
		s_windowClass.cbWndExtra = 0;
		s_windowClass.hInstance = GetModuleHandle(NULL);
		s_windowClass.hIcon = NULL;
		s_windowClass.hCursor = LoadCursor(s_windowClass.hInstance, IDC_ARROW);
		s_windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		s_windowClass.lpszMenuName = NULL;
		s_windowClass.lpszClassName = s_windowClassName;
		s_windowClass.hIconSm = NULL;

		s_windowClassIsRegistered = RegisterClassEx(&s_windowClass);
	}
}

Window::~Window()
{

}

#pragma endregion

#pragma region "Public Methods"

void Window::MakeCurrent() const
{
	wglMakeCurrent(m_device, m_context);
}

void Window::SwapBuffers() const
{
	::SwapBuffers(m_device);
}

bool Window::Create(int width, int height, char const * title)
{
	if (!s_windowClassIsRegistered)
		return false;

	m_handle = CreateWindowEx(0, s_windowClassName, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);

	if (!m_handle)
		return false;

	m_width = width;
	m_height = height;

	UpdateWindow(m_handle);
	ShowWindow(m_handle, SW_SHOW);

	m_device = GetDC(m_handle);

	PIXELFORMATDESCRIPTOR pfd;
	std::memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 0;

	int formatIndex = ChoosePixelFormat(m_device, &pfd);
	SetPixelFormat(m_device, formatIndex, &pfd);

	m_context = wglCreateContext(m_device);
	return true;
}

void Window::Destroy()
{
	if (m_handle) {
		wglMakeCurrent(m_device, NULL);
		wglDeleteContext(m_context);
		UnregisterClass(s_windowClassName, GetModuleHandle(NULL));
		DestroyWindow(m_handle);
		m_handle = 0;
	}
}

#pragma endregion

#pragma region "Getters"

HWND const & Window::GetHandle() const
{
	return m_handle;
}

#pragma endregion