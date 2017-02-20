#include <Framework/Input.h>
#include <Framework/Application.h>
#include <imgui/imgui.h>

Input::Input(Application * application) : m_application(application)
{
}

Input::~Input()
{
}

void Input::Initialize()
{
	m_io = &ImGui::GetIO();
}

void Input::Update()
{
	if (!m_io->WantCaptureMouse)
	{
		if(m_io->MouseDown[0])
			m_application->MouseDragged(Application::LEFT_MOUSE_BUTTON, (int)m_io->MouseDelta.x, (int)m_io->MouseDelta.y);
		if (m_io->MouseDown[1])
			m_application->MouseDragged(Application::RIGHT_MOUSE_BUTTON, (int)m_io->MouseDelta.x, (int)m_io->MouseDelta.y);
		
		m_application->MouseScrolled(m_io->MouseWheel);
	}
}

void Input::Finalize()
{

}

void Input::KeyDown(unsigned char key)
{
	m_io->KeysDown[key] = true;
	if (key == VK_LCONTROL || key == VK_CONTROL || key == VK_RCONTROL)
		m_io->KeyCtrl = true;
	if (key == VK_LMENU || key == VK_RMENU || key == VK_MENU)
		m_io->KeyAlt = true;
	if (key == VK_LSHIFT || key == VK_RSHIFT || key == VK_SHIFT)
		m_io->KeyShift = true;
}

void Input::KeyUp(unsigned char key)
{
	m_io->KeysDown[key] = false;
	if (key == VK_LCONTROL || key == VK_CONTROL || key == VK_RCONTROL)
		m_io->KeyCtrl = false;
	if (key == VK_LMENU || key == VK_RMENU || key == VK_MENU)
		m_io->KeyAlt = false;
	if (key == VK_LSHIFT || key == VK_RSHIFT || key == VK_SHIFT)
		m_io->KeyShift = false;
}

void Input::MouseMove(int x, int y)
{
	m_io->MousePos.x = x;
	m_io->MousePos.y = y;
}

void Input::MouseDown(char button)
{
	m_io->MouseDown[button] = true;
}

void Input::MouseUp(char button)
{
	m_io->MouseDown[button] = false;
}

void Input::MouseScroll(int value)
{
	m_io->MouseWheel = (float)value / 120.0f;
}

void Input::CharInput(int character)
{
	m_io->AddInputCharacter(character);
}