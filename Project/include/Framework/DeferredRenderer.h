#pragma once

#include <Framework/IRenderer.h>
#include <Framework/Program.h>

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

private:

	void RenderNode(Node const * const & node, glm::mat4 modelMatrix, std::vector<std::pair<Light const *, glm::vec3>> & lights) const;
	void GenerateShadowMap(Light const * light) const;

	void CreateGBuffer(int const & width, int const & height);
	void CreateShadowFramebuffer(int const & width, int const & height);
	void FreeGBuffer();
	void BindGBuffer() const;
	void BindDefaultFramebuffer() const;
	void BindShadowFramebuffer(unsigned int shadowMap) const;

	struct gBuffer
	{
		unsigned int framebuffer;
		unsigned int colorBuffers[4];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers[4];
	} m_gBuffer;

	struct DefaultFramebuffer
	{
		unsigned int framebuffer;
		unsigned int width;
		unsigned int height;
		unsigned int drawBuffers;
	} m_defaultFramebuffer;

	struct ShadowFramebuffer
	{
		unsigned int framebuffer;
		unsigned int width;
		unsigned int height;
		unsigned int depthBuffer;
	} m_shadowFramebuffer;

	struct lightGeometry
	{
		unsigned int fsqVAO;
		unsigned int fsqVBO;
		unsigned int circleVAO;
		unsigned int cirlceVBO;
	} m_lightGeometry;

	Program m_deferredPassProgram;
	Program m_lightingPassProgram;

};