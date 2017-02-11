#pragma once

#include <Framework/IRenderer.h>
#include <Framework/Program.h>

class DeferredRenderer : public IRenderer
{
public:
	
	//constructors/destructor
	DeferredRenderer();
	~DeferredRenderer();

	//public methods
	bool Initialize();
	void Finalize();
	void RenderScene(Scene const & scene) const;

private:

	struct gBuffer
	{
		unsigned int famebuffer;
		unsigned int colorBuffers[4];
		unsigned int depthBuffer;
	} m_gBuffer;

	struct lightGeometry
	{
		unsigned int fsqVAO;
		unsigned int fsqVBO;
	} m_lightGeometry;

	Program m_program;

};