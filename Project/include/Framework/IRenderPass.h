#pragma once

#include <glm/glm.hpp>

class Node;
class Scene;
class IRenderer;

class IRenderPass
{
public:
	IRenderPass(IRenderer const * renderer) : m_renderer(renderer) {}
	virtual ~IRenderPass() {}
	virtual void Initialize() = 0;
	virtual void ProcessNode(Node const * const & node, glm::mat4 const & modelMatrix) const = 0;
	virtual void Finalize() = 0;
protected:
	IRenderer const * m_renderer;
};