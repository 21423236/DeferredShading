#pragma once

#include <Framework/Window.h>
#include <Framework/Scene.h>
#include <Framework/IRenderer.h>

class Application
{
public:

	//constructors/destructor
	Application(IRenderer * renderer);
	~Application();

	//events
	void MouseDown();
	void MouseUp();
	void MouseMove();
	void KeyDown();
	void KeyUp();

	void RenderFrame() const;

	void Run() const;

private:

	Window * m_window;
	Scene * m_scene;
	IRenderer * m_renderer;

};