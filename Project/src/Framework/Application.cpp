#include <Framework/Application.h>
#include <Framework/Object.h>
#include <Framework/GlobalLight.h>
#include <Framework/LocalLight.h>
#include <Framework/Shape.h>
#include <GL/glew.h>
#include <Windows.h>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

#define M_PI 3.14159265358979323846f

#pragma region "Static Data"

std::map<HWND, Application*> Application::s_applicationDictionary;

#pragma endregion

#pragma region "Constructors/Destructor"

Application::Application(IRenderer * renderer) : m_window(new Window(WndProcRouter)), m_scene(new Scene(800, 600)), m_renderer(renderer), m_gui(new GUI()), m_input(new Input(this)), m_running(false), m_isPaused(false), m_width(800), m_height(600)
{

}


Application::~Application()
{
	delete m_input;
	delete m_gui;
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

	m_gui->Initialize(*m_window);
	m_input->Initialize();
	
	Initialize();

	MSG msg;
	std::memset(&msg, 0, sizeof(MSG));

	while (m_running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_input->Update();

		if (!m_isPaused)
		{
			m_gui->NewFrame(m_width, m_height);

			//render a frame
			RenderFrame();

			//render gui
			m_gui->GenerateGUI(*m_scene);
			m_renderer->GenerateGUI();
			m_gui->EndFrame();

			//swap window's back and front buffers
			m_window->SwapBuffers();
		}
	}

	//finalize renderer and destroy window
	Shape::FreeMemory();
	m_input->Finalize();
	m_gui->Finalize();
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

void Application::Initialize()
{
	Mesh * bunnyMesh = m_scene->CreateMesh("bunny", "Resources/Meshes/dragon.obj");
	Material * bunnyMaterial = m_scene->CreateMaterial("bunny", glm::vec3(1, 1, 1), glm::vec3(0.04f, 0.04f, 0.04f), 2);

	Object * bunnyObject1 = new Object("bunny1", bunnyMesh, bunnyMaterial);
	Object * bunnyObject2 = new Object("bunny2", bunnyMesh, bunnyMaterial);
	Object * bunnyObject3 = new Object("bunny3", bunnyMesh, bunnyMaterial);
	bunnyObject1->SetTranslation(glm::vec3(-2.0f, 0.0f, 0));
	bunnyObject1->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	bunnyObject2->SetTranslation(glm::vec3(0, 0.0f, 0));
	bunnyObject2->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	bunnyObject3->SetTranslation(glm::vec3(2.0f, 0.0f, 0));
	bunnyObject3->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

	Mesh * planeMesh = m_scene->CreateMesh("plane", "Resources/Meshes/plane.obj");
	Material * planeMaterial = m_scene->CreateMaterial("plane", glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.02f, 0.02f, 0.02f), 20);

	Object * planeObject = new Object("plane", planeMesh, planeMaterial);
	planeObject->SetScale(glm::vec3(8, 8, 8));
	planeObject->AddChild(bunnyObject1);
	planeObject->AddChild(bunnyObject2);
	planeObject->AddChild(bunnyObject3);

	m_scene->AddNode(planeObject);

	/*Node * lights = new Node("local_lights", glm::vec3(0, 0, 0), glm::quat());
	for (int r = 0; r < 100; r++)
	{
		for (int c = 0; c < 100; c++)
		{
			LocalLight * localLight = new LocalLight(std::to_string((r*100) + c), glm::vec3(sinf(c*r/10000.0f*M_PI), sinf(c/100.0f*M_PI), sinf(r/100.0f*M_PI)), 1.0f);
			localLight->SetTranslation(glm::vec3(-3.5 + (7.0f / 100.0f) * c, sinf(r/100.0f * M_PI)*0.05f, -3.5 + (7.0f / 100.0f) * r));
			localLight->SetRadius(0.5f + sinf(r/100.0f * M_PI)*0.25f);
			lights->AddChild(localLight);
		}
	}
	m_scene->AddNode(lights);*/ 

	LocalLight * localLight1 = new LocalLight("local1", glm::vec3(4, 0, 0), 1.0f);
	localLight1->SetTranslation(glm::vec3(-2.0f, 1.0f, 0));
	m_scene->AddNode(localLight1);

	LocalLight * localLight2 = new LocalLight("local2", glm::vec3(0, 4, 0), 1.0f);
	localLight2->SetTranslation(glm::vec3(0.0f, 1.0f, 0.0f));
	m_scene->AddNode(localLight2);

	LocalLight * localLight3 = new LocalLight("local3", glm::vec3(0, 0, 4), 1.0f);
	localLight3->SetTranslation(glm::vec3(2.0f, 1.0f, 0.0f));
	m_scene->AddNode(localLight3);

	GlobalLight * globalLight1 = new GlobalLight("global1", glm::vec3(1, 1, 1));
	globalLight1->SetTranslation(glm::vec3(4, 10, -4));
	m_scene->AddNode(globalLight1);

	GlobalLight * globalLight2 = new GlobalLight("global2", glm::vec3(1, 1, 1));
	globalLight2->SetTranslation(glm::vec3(-4, 10, 4));
	m_scene->AddNode(globalLight2);

	m_scene->SetProjection(0.2f, 0.1f, 1000.0f);
	m_scene->SetSceneSize(glm::vec3(14.0f, 14.0f, 14.0f));
	m_scene->SetAmbientIntensity(glm::vec3(0.1f, 0.1f, 0.1f));
	m_scene->GetCamera().SetZoom(10.0f);
}

void Application::MouseClicked(MouseButton button, int x, int y)
{

}

void Application::MouseDragged(MouseButton button, int dx, int dy)
{
	Camera & camera = m_scene->GetCamera();
	switch (button)
	{
	case LEFT_MOUSE_BUTTON:
		camera.SetSpin(camera.GetSpin() - (((float)dx / (float)m_window->m_width) * M_PI));
		camera.SetTilt(camera.GetTilt() - (((float)dy / (float)m_window->m_height) * M_PI));
		break;
	case RIGHT_MOUSE_BUTTON:
		camera.SetPosition(camera.GetPosition().x + ((float)dx / (2.0f * (float)m_window->m_width)), camera.GetPosition().y - ((float)dy / (2.0f*(float)m_window->m_height)), 0.0f);
		break;
	case MIDDLE_MOUSE_BUTTON:

		break;
	}
}

void Application::MouseScrolled(int delta)
{
	m_scene->GetCamera().SetZoom(m_scene->GetCamera().GetZoom() - delta);
}

#pragma endregion

#pragma region "Private Methods"

LRESULT Application::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO & io = ImGui::GetIO();
	switch (msg)
	{
	case WM_CLOSE:
		//inform the application to stop running
		m_running = false;
		break;
	case WM_LBUTTONDOWN:
		m_input->MouseDown(0);
		break;
	case WM_LBUTTONUP:
		m_input->MouseUp(0);
		break;
	case WM_RBUTTONDOWN:
		m_input->MouseDown(1);
		break;
	case WM_RBUTTONUP:
		m_input->MouseUp(1);
		break;
	case WM_MOUSEMOVE:
		m_input->MouseMove(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		m_input->KeyDown(wParam);
		break;
	case WM_KEYUP:
		m_input->KeyUp(wParam);
		break;
	case WM_MOUSEWHEEL:
		m_input->MouseScroll((int16_t)HIWORD(wParam));
		break;
	case WM_ENTERSIZEMOVE:
		m_isPaused = true;
		break;
	case WM_EXITSIZEMOVE:
		m_isPaused = false;
		m_renderer->Resize(m_width, m_height);
		m_scene->Resize(m_width, m_height);
		break;
	case WM_SIZE:
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);

		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
		{
			m_renderer->Resize(m_width, m_height);
			m_scene->Resize(m_width, m_height);
		}
		break;
	case WM_CHAR:
		m_input->CharInput(wParam);
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