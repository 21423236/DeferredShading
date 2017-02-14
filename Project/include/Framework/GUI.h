#pragma once

class Window;
class Scene;
class Node;
class Material;

class GUI
{
public:

	GUI();
	~GUI();

	void Initialize(Window const & window);
	void NewFrame(int const & width, int const & height);
	void GenerateGUI(Scene & scene);
	void EndFrame();
	void Finalize();

private:

	void TraverseGraph(Node * node);
	void MaterialEditor(Material * material);
	void NodeInformation(Node * node);

};

