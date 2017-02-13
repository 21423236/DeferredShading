#pragma once

class Window;
class Scene;

class GUI
{
public:

	GUI();
	~GUI();

	void Initialize(Window const & window);
	void NewFrame(int const & width, int const & height);
	void GenerateGUI(Scene const & scene);
	void EndFrame();
	void Finalize();

};

