#include <Framework/LightingPass.h>
#include <Framework/Program.h>
#include <Framework/GlobalLight.h>
#include <Framework/LocalLight.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Scene.h>
#include <Framework/Shape.h>

#include <GL/glew.h>
#include <cmath>

#pragma region "Constructors/Destructor"

LightingPass::LightingPass(IRenderer const * renderer) : m_renderer(renderer), m_globalLightProgram(), m_localLightProgram()
{
}

LightingPass::~LightingPass()
{
}

#pragma endregion

#pragma region "Public Methods"

void LightingPass::Initialize()
{
	m_globalLightProgram.CreateHandle();
	m_globalLightProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/GlobalLightPass.vert");
	m_globalLightProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/GlobalLightPass.frag");
	m_globalLightProgram.Link();

	m_globalLightProgram.SetUniform("uColor0", 1);
	m_globalLightProgram.SetUniform("uColor1", 2);
	m_globalLightProgram.SetUniform("uColor2", 3);
	m_globalLightProgram.SetUniform("uColor3", 4);
	m_globalLightProgram.SetUniform("uShadow.map", 5);

	m_localLightProgram.CreateHandle();
	m_localLightProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/LocalLightPass.vert");
	m_localLightProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/LocalLightPass.frag");
	m_localLightProgram.Link();

	m_localLightProgram.SetUniform("uColor0", 1);
	m_localLightProgram.SetUniform("uColor1", 2);
	m_localLightProgram.SetUniform("uColor2", 3);
	m_localLightProgram.SetUniform("uColor3", 4);

}

void LightingPass::Prepare(Scene const & scene, glm::vec2 const & windowSize) const
{
	DeferredRenderer const * deferredRenderer = dynamic_cast<DeferredRenderer const *>(m_renderer);
	deferredRenderer->BlitDepthBuffers();
	deferredRenderer->BindDefaultFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	glm::vec3 eyePosition = glm::vec3(glm::inverse(scene.GetViewMatrix()) * glm::vec4(0, 0, 0, 1));

	m_globalLightProgram.SetUniform("uWindowSize", windowSize);
	m_globalLightProgram.SetUniform("uEye", eyePosition);

	m_localLightProgram.SetUniform("uWindowSize", windowSize);
	m_localLightProgram.SetUniform("uEye", eyePosition);
	m_localLightProgram.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_localLightProgram.SetUniform("uViewMatrix", scene.GetViewMatrix());
}

void LightingPass::ProcessGlobalLights(std::vector<std::pair<GlobalLight const *,glm::vec3>> const & globalLights) const
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	m_globalLightProgram.Use();

	glBindVertexArray(Shape::GetFullScreenQuad()->GetVAO());
	glEnableVertexAttribArray(0);

	for (auto const & lightPair : globalLights)
	{
		m_globalLightProgram.SetUniform("uLight.position", lightPair.second);
		m_globalLightProgram.SetUniform("uLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		m_globalLightProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		m_globalLightProgram.SetUniform("uShadow.matrix", lightPair.first->GetShadowMatrix());
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, lightPair.first->GetShadowMap()); 

		glDrawElements(GL_TRIANGLES, Shape::GetFullScreenQuad()->GetIndexCount()*3, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void LightingPass::ProcessLocalLights(std::vector<std::pair<LocalLight const *, glm::vec3>> const & localLights) const
{
	glEnable(GL_DEPTH_TEST);

	m_localLightProgram.Use();

	glBindVertexArray(Shape::GetIcosahedron()->GetVAO());
	glEnableVertexAttribArray(0);

	for (auto const & lightPair : localLights)
	{
		m_localLightProgram.SetUniform("uLight.position", lightPair.second);
		m_localLightProgram.SetUniform("uLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		m_localLightProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());
		m_localLightProgram.SetUniform("uLight.radius", lightPair.first->GetRadius());

		glDrawElements(GL_TRIANGLES, Shape::GetIcosahedron()->GetIndexCount()*3, GL_UNSIGNED_INT, 0);
	}
	
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	
}

void LightingPass::Finalize()
{
	m_localLightProgram.DestroyHandle();
	m_globalLightProgram.DestroyHandle();
}

#pragma endregion