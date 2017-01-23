#pragma once
class Application
{
public:
	Application();
	~Application();

	//events
	void MouseDown();
	void MouseUp();
	void MouseMove();
	void KeyDown();
	void KeyUp();
};