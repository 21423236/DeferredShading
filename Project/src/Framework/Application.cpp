#include <Framework/Application.h>
#include <GL/glew.h>

#pragma region "Static Data"

std::map<HWND, Application*> Application::s_applicationDictionary;

#pragma endregion

#pragma region "Constructors/Destructor"

Application::Application(IRenderer * renderer) : m_window(new Window(WndProcRouter)), m_scene(new Scene()), m_renderer(renderer), m_running(false)
{

}


Application::~Application()
{
	delete m_renderer;
	delete m_scene;
	delete m_window;
}

#pragma endregion

#pragma region "Public Methods"

void Application::RenderFrame() const
{
	m_renderer->RenderScene(*m_scene);
}

int Application::Run()
{
	int retValue = 0;
	m_running = true;
	
	if (!m_window->Create(800, 600, "Deferred Rendering"))
	{
		MessageBox(NULL, "Failed to create window", "Error!", MB_OK | MB_ICONHAND);
		m_running = false;
		retValue = 1;
	}
	else {
		//register this application instance to handle m_window's WndProc
		s_applicationDictionary[m_window->GetHandle()] = this;
		m_window->MakeCurrent();
	}
	
	//load opengl functions
	if (m_running && glewInit() != GLEW_OK)
	{
		MessageBox(NULL, "Failed to initialize glew32", "Error!", MB_OK | MB_ICONHAND);
		m_running = false;
		retValue = 1;
	}

	//initialize the renderer
	if (m_running && !m_renderer->Initialize())
	{
		MessageBox(NULL, "Failed to initialize renderer", "Error!", MB_OK | MB_ICONHAND);
		m_running = false;
		retValue = 1;
	}

	MSG msg;
	std::memset(&msg, 0, sizeof(MSG));

	while (m_running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//render a frame
		RenderFrame();

		//swap window's back and front buffers
		m_window->SwapBuffers();
	}

	//finalize renderer and destroy window
	m_renderer->Finalize();
	m_window->Destroy();

	return retValue;
}

float Application::dt() const
{
	return 0.0f;
}

#pragma endregion

#pragma region "Private Methods"

LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		//inform the application to stop running
		m_running = false;
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

#pragma endregion

#pragma region "Static Functions"

LRESULT Application::WndProcRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (s_applicationDictionary.count(hWnd))
		return s_applicationDictionary[hWnd]->WndProc(hWnd, msg, wParam, lParam);
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#pragma endregion