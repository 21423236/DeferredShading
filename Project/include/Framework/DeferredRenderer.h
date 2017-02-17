#pragma once

#include <Framework/IRenderer.h>
#include <Framework/Program.h>
#include <Framework/DeferredPass.h>
#include <Framework/LightingPass.h>
#include <Framework/ShadowPass.h>

#include <vector>

class Light;

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
	void Resize(int const & width, int const & height);
	void GenerateGUI();

	void BindGBuffer() const;
	void BindDefaultFramebuffer() const;
	void BindShadowBuffer(unsigned int const & shadowTexture) const;

private:
	
	void CreateGBuffer(int const & width, int const & height);
	void FreeGBuffer();
	void CreateShadowBuffer(int const & width, int const & height);
	void FreeShadowBuffer();

	struct gBuffer
	{
		unsigned int framebuffer;
		unsigned int colorBuffers[4];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers[4];
	} m_gBuffer;

	struct ShadowBuffer
	{
		unsigned int framebuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers;
	} m_shadowBuffer;

	struct DefaultFramebuffer
	{
		unsigned int framebuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers;
	} m_defaultFramebuffer;

	DeferredPass m_deferredPass;
	ShadowPass m_shadowPass;
	LightingPass m_lightingPass;

};