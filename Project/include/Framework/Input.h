#pragma once

#include <imgui/imgui.h>

class Application;

class Input
{

public:

	Input(Application * application);
	~Input();

	void Initialize();
	void Update();
	void Finalize();

	void KeyDown(unsigned char key);
	void KeyUp(unsigned char key);
	void MouseMove(int x, int y);
	void MouseDown(char button);
	void MouseUp(char button);
	void MouseScroll(int value);
	void CharInput(int character);

private:

	Application * m_application;

	ImGuiIO * m_io;

};

