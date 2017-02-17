#include <Framework/LightingPass.h>
#include <Framework/Program.h>
#include <Framework/Light.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Scene.h>

#include <GL/glew.h>

#pragma region "Constructors/Destructor"

LightingPass::LightingPass(IRenderer const * renderer) : m_renderer(renderer), m_lightingProgram()
{
}

LightingPass::~LightingPass()
{
}

#pragma endregion

#pragma region "Public Methods"

void LightingPass::Initialize()
{
	m_lightingProgram.CreateHandle();
	m_lightingProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/LightingPass.vert");
	m_lightingProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/LightingPass.frag");
	m_lightingProgram.Link();

	m_lightingProgram.SetUniform("uColor0", 1);
	m_lightingProgram.SetUniform("uColor1", 2);
	m_lightingProgram.SetUniform("uColor2", 3);
	m_lightingProgram.SetUniform("uColor3", 4);
	m_lightingProgram.SetUniform("uShadow.shadowTexture", 5);

	glGenVertexArrays(1, &m_lightGeometries.fsqVAO);
	glBindVertexArray(m_lightGeometries.fsqVAO);
	glGenBuffers(1, &m_lightGeometries.fsqVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometries.fsqVBO);

	float fsqData[] = {
		-1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(fsqData), fsqData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
	glBindVertexArray(0);
}

void LightingPass::Prepare(Scene const & scene, glm::vec2 const & windowSize) const
{
	dynamic_cast<DeferredRenderer const *>(m_renderer)->BindDefaultFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	m_lightingProgram.Use();
	m_lightingProgram.SetUniform("uWindowSize", windowSize);
	m_lightingProgram.SetUniform("uEye", glm::vec3(glm::inverse(scene.GetViewMatrix()) * glm::vec4(0, 0, 0, 1)));
}

void LightingPass::ProcessGlobalLights(std::vector<std::pair<Light const *,glm::vec3>> const & globalLights) const
{
	m_lightingProgram.SetUniform("uLight.isGlobal", 1);

	for (auto lightPair : globalLights)
	{
		m_lightingProgram.SetUniform("uLight.position", lightPair.second);
		m_lightingProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_lightingProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		m_lightingProgram.SetUniform("uShadow.shadowMatrix", lightPair.first->GetShadowMatrix());
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, lightPair.first->GetShadowTexture()); 

		glBindVertexArray(m_lightGeometries.fsqVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

	}
}

void LightingPass::ProcessLocalLights(std::vector<std::pair<Light const *, glm::vec3>> const & localLights) const
{
	m_lightingProgram.SetUniform("uLight.isGlobal", 0);
}

void LightingPass::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometries.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometries.fsqVAO);
	m_lightingProgram.DestroyHandle();
}

#pragma endregion