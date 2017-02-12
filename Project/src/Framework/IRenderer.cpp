#include <Framework/IRenderer.h>
#include <Framework/Node.h>
#include <Framework/Scene.h>

Node const * const & IRenderer::GetRootNode(Scene const & scene) const
{
	return scene.m_rootNode;
}

std::vector<Node *> const & IRenderer::GetChildren(Node const * const & node) const
{
	return node->m_children;
}