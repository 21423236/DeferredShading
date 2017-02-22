#pragma once

#include <Framework/IRenderer.h>
#include <Framework/Program.h>
#include <Framework/DeferredPass.h>
#include <Framework/LightingPass.h>
#include <Framework/ShadowPass.h>
#include <Framework/Texture.h>
#include <Framework/UniformBuffer.h>
#include <Framework/ShaderStorageBuffer.h>

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
	void BindShadowBuffer(Texture const & shadowTexture) const;
	void BindLightAccumulationBuffer() const;
	void BindDefaultFramebuffer() const;
	void BlitDepthBuffers() const;

private:
	
	void CreateGBuffer(int const & width, int const & height);
	void FreeGBuffer();
	void CreateShadowBuffer(int const & width, int const & height);
	void FreeShadowBuffer();
	void CreateLightAccumulationBuffer(int const & width, int const & height);
	void FreeLightAccumulationBuffer();

	struct gBuffer
	{
		unsigned int framebuffer;
		Texture colorBuffer0;
		Texture colorBuffer1;
		Texture colorBuffer2;
		Texture colorBuffer3;
		Texture depthBuffer;
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

	struct LightAccumulationBuffer
	{
		unsigned int framebuffer;
		Texture colorBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers;
	} m_lightAccumulationBuffer;

	struct DefaultFramebuffer
	{
		unsigned int framebuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers;
	} m_defaultFramebuffer;

	mutable UniformBuffer										m_sceneUniformBuffer;
	mutable ShaderStorageBuffer<struct LocalLightInformation>	m_localLightsBuffer;

	Program m_debugProgram;

	DeferredPass m_deferredPass;
	ShadowPass m_shadowPass;
	LightingPass m_lightingPass;

	bool m_gatherStatistics;
	bool m_displayLightVolumes;

};