#include <Framework/ToneMappingPass.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Shape.h>

ToneMappingPass::ToneMappingPass(IRenderer const * renderer) : m_renderer(renderer), m_toneMappingProgram(), m_method(REINHARD), m_gamma(2.2f), m_exposure(1.0f)
{
}


ToneMappingPass::~ToneMappingPass()
{
}

#pragma region "Public Methods"

void ToneMappingPass::Initialize()
{
	m_toneMappingProgram.CreateHandle();
	m_toneMappingProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/ToneMappingPass.vert");
	m_toneMappingProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/ToneMappingPass.frag");
	m_toneMappingProgram.Link();

	m_toneMappingProgram.SetUniform("uFrameTexture", 6);
	m_toneMappingProgram.SetUniform("uGamma", m_gamma);
	m_toneMappingProgram.SetUniform("uMethod", m_method);
	m_toneMappingProgram.SetUniform("uExposure", m_exposure);
}

void ToneMappingPass::Prepare() const
{
	static DeferredRenderer const * renderer = dynamic_cast<DeferredRenderer const *>(m_renderer);
	renderer->BindDefaultFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	m_toneMappingProgram.Use();

	m_toneMappingProgram.SetUniform("uGamma", m_gamma);
	m_toneMappingProgram.SetUniform("uMethod", m_method);
	m_toneMappingProgram.SetUniform("uExposure", m_exposure);
}

void ToneMappingPass::ProcessFrame() const
{
	glBindVertexArray(Shape::GetFullScreenQuad()->GetVAO());
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, Shape::GetFullScreenQuad()->GetIndexCount(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void ToneMappingPass::Finalize()
{
	m_toneMappingProgram.DestroyHandle();
}

#pragma endregion