#include <Framework/DeferredRenderer.h>
#include <GL/glew.h>

#pragma region "Constructors/Destructor"

DeferredRenderer::DeferredRenderer()
{
}


DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::Initialize()
{
	glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void DeferredRenderer::Finalize()
{

}