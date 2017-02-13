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

private:

	Application * m_application;

	struct Mouse {
		int x;
		int y;
		int dx;
		int dy;
		bool left;
		bool right;
		bool middle;
	} m_mouseState;

	bool m_previousFrameKeys[512];
	bool m_currentFrameKeys[512];

	ImGuiIO * m_io;

};

