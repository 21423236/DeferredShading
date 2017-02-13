#pragma once

#include <vector>

class Scene;
class Node;

class IRenderer
{
public:

	virtual ~IRenderer() {}
	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void RenderScene(Scene const & scene) const = 0;
	virtual void Resize(int const & width, int const & height) = 0;
	virtual void GenerateGUI() = 0;

protected:

	Node const * const & GetRootNode(Scene const & scene) const;
	std::vector<Node *> const & GetChildren(Node const * const & node) const;

};