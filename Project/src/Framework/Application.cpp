#include <Framework/Application.h>
#include <GL/glew.h>
#include <Windows.h>
#include <iostream>

#define M_PI 3.14159265358979323846f

#pragma region "Static Data"

std::map<HWND, Application*> Application::s_applicationDictionary;

#pragma endregion

#pragma region "Constructors/Destructor"

Application::Application(IRenderer * renderer) : m_window(new Window(WndProcRouter)), m_scene(new Scene(800, 600)), m_renderer(renderer), m_running(false), m_leftState({ false, 0, 0, 0, 0 }), m_rightState({false, 0, 0, 0, 0})
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
	AllocConsole();
	FILE * file;
	freopen_s(&file, "CONOUT$", "w", stdout);

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

	m_scene->LoadObject("C:/Users/david/Desktop/bunny.obj");
	m_scene->SetProjection(0.2f, 0.1f, 20.0f);
	m_scene->GetCamera().SetZoom(1.0f);

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
	m_scene->FreeMemory();
	m_renderer->Finalize();
	m_window->Destroy();

	return retValue;
}

float Application::dt() const
{
	return 0.0f;
}

#pragma endregion

#pragma region "Events"

void Application::MouseClicked(MouseButton button, int x, int y)
{

}

void Application::MouseDragged(MouseButton button, int dx, int dy)
{
	Camera & camera = m_scene->GetCamera();
	switch (button)
	{
	case LEFT_MOUSE_BUTTON:
		camera.SetSpin(camera.GetSpin() + (((float)dx / (float)m_window->m_width) * M_PI));
		camera.SetTilt(camera.GetTilt() + (((float)dy / (float)m_window->m_height) * M_PI));
		break;
	case RIGHT_MOUSE_BUTTON:

		break;
	case MIDDLE_MOUSE_BUTTON:

		break;
	}
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
	case WM_LBUTTONDBLCLK:
		MouseClicked(LEFT_MOUSE_BUTTON, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_LBUTTONDOWN:
		m_leftState.down = true;
		m_leftState.previousX = LOWORD(lParam);
		m_leftState.previousY = HIWORD(lParam);
		m_leftState.deltaX = 0;
		m_leftState.deltaY = 0;
		break;
	case WM_LBUTTONUP:
		m_leftState.down = false;
		break;
	case WM_RBUTTONDBLCLK:
		MouseClicked(RIGHT_MOUSE_BUTTON, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_RBUTTONDOWN:
		m_rightState.down = true;
		m_rightState.previousX = LOWORD(lParam);
		m_rightState.previousY = HIWORD(lParam);
		m_rightState.deltaX = 0;
		m_rightState.deltaY = 0;
		break;
	case WM_RBUTTONUP:
		m_rightState.down = false;
		break;
	case WM_MOUSEMOVE:
		if (m_leftState.down)
		{
			int currentX = LOWORD(lParam);
			int currentY = HIWORD(lParam);
			m_leftState.deltaX = currentX - m_leftState.previousX;
			m_leftState.deltaY = currentY - m_leftState.previousY;
			m_leftState.previousX = currentX;
			m_leftState.previousY = currentY;
			MouseDragged(LEFT_MOUSE_BUTTON, m_leftState.deltaX, m_leftState.deltaY);
		}
		if (m_rightState.down)
		{
			int currentX = LOWORD(lParam);
			int currentY = HIWORD(lParam);
			m_rightState.deltaX = currentX - m_rightState.previousX;
			m_rightState.deltaY = currentY - m_rightState.previousY;
			m_rightState.previousX = currentX;
			m_rightState.previousY = currentY;
			MouseDragged(RIGHT_MOUSE_BUTTON, m_rightState.deltaX, m_rightState.deltaY);
		}
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