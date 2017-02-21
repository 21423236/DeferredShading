#pragma once

class Window;
class Scene;
class Node;
class Material;
class Object;
class GlobalLight;
class LocalLight;

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

	void TraverseNode(Node * node, Scene & scene);
	void MaterialEditor(Material * material, Scene & scene);
};

