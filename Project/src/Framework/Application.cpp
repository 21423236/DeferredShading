#include <Framework/Application.h>
#include <GL/glew.h>

Application::Application(IRenderer * renderer) : m_window(new Window(800, 600, "Deferred Rendering")), m_scene(new Scene()), m_renderer(renderer)
{
	m_window->MakeCurrent();
	if (glewInit() != GLEW_OK)
	{
		MessageBox(NULL, "Failed to initialize glew", "Error!", MB_OK | MB_ICONHAND);
	}
}


Application::~Application()
{
	delete m_renderer;
	delete m_scene;
	delete m_window;
}

#pragma region "Events"

void Application::MouseDown()
{

}

void Application::MouseUp()
{

}

void Application::MouseMove()
{

}

void Application::KeyDown()
{

}

void Application::KeyUp()
{

}

void Application::RenderFrame() const
{
	m_renderer->RenderScene(*m_scene);
}

void Application::Run() const
{

}

#pragma endregion