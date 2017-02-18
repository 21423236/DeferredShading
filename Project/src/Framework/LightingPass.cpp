#include <Framework/LightingPass.h>
#include <Framework/Program.h>
#include <Framework/Light.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Scene.h>

#include <GL/glew.h>
#include <cmath>

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
	m_lightingProgram.SetUniform("uShadow.shadowTexture", 6);

	//generate full screen quad
	glGenVertexArrays(1, &m_lightGeometries.fsqVAO);
	glBindVertexArray(m_lightGeometries.fsqVAO);
	glGenBuffers(1, &m_lightGeometries.fsqVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometries.fsqVBO);

	float fsqData[] = {
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(fsqData), fsqData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glBindVertexArray(0);

	float const t = (1.0f + sqrt(5.0f)) / 2.0f;

	float icoBaseVertices[12][3] = {
		{-1.0f, t, 0.0f},
		{ 1.0f, t, 0.0f},
		{ -1.0f, -t, 0.0f},
		{ 1.0f, -t, 0.0f},

		{ 0.0f, -1.0f, t},
		{ 0.0f, 1.0f, t },
		{ 0.0f, -1.0f, -t },
		{ 0.0f , 1.0f, -t },

		{ t, 0.0f, -1.0f },
		{ t, 0.0f, 1.0f },
		{ -t, 0.0f, -1.0f },
		{ -t, 0.0f, 1.0f }
	};

	int faces[20][3] = {
		{0, 11, 5},
		{0, 5, 1},
		{0, 1, 7},
		{0, 7, 10},
		{0, 10, 11},

		{1, 5, 9},
		{5, 11, 4},
		{11, 10, 2},
		{10, 7, 6},
		{7, 1, 8},

		{3, 9, 4},
		{3, 4, 2},
		{3, 2, 6},
		{3, 6, 8},
		{3, 8, 9},

		{4, 9, 5},
		{2, 4, 11},
		{6, 2, 10},
		{8, 6, 7},
		{9, 8, 1}
	};

	glGenVertexArrays(1, &m_lightGeometries.sphereVAO);
	glBindVertexArray(m_lightGeometries.sphereVAO);

	glGenBuffers(1, &m_lightGeometries.sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometries.sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, icoBaseVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_lightGeometries.sphereIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lightGeometries.sphereIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 60, &faces, GL_STATIC_DRAW);
	m_lightGeometries.sphereIndexCount = 60;

	glBindVertexArray(0);
}

void LightingPass::Prepare(Scene const & scene, glm::vec2 const & windowSize) const
{
	DeferredRenderer const * deferredRenderer = dynamic_cast<DeferredRenderer const *>(m_renderer);
	deferredRenderer->BlitDepthBuffers();
	deferredRenderer->BindDefaultFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	m_lightingProgram.Use();
	m_lightingProgram.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_lightingProgram.SetUniform("uViewMatrix", scene.GetViewMatrix());
	m_lightingProgram.SetUniform("uWindowSize", windowSize);
	m_lightingProgram.SetUniform("uEye", glm::vec3(glm::inverse(scene.GetViewMatrix()) * glm::vec4(0, 0, 0, 1)));
}

void LightingPass::ProcessGlobalLights(std::vector<std::pair<Light const *,glm::vec3>> const & globalLights) const
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	m_lightingProgram.SetUniform("uLight.isGlobal", true);

	for (auto const & lightPair : globalLights)
	{
		m_lightingProgram.SetUniform("uLight.position", lightPair.second);
		m_lightingProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_lightingProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		m_lightingProgram.SetUniform("uShadow.shadowMatrix", lightPair.first->GetShadowMatrix());
		glActiveTexture(GL_TEXTURE6);
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
	//glEnable(GL_DEPTH_TEST);

	m_lightingProgram.SetUniform("uLight.isGlobal", false);

	for (auto const & lightPair : localLights)
	{
		m_lightingProgram.SetUniform("uLight.position", lightPair.second);
		m_lightingProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_lightingProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());
		m_lightingProgram.SetUniform("uLight.radius", lightPair.first->GetRadius());

		glBindVertexArray(m_lightGeometries.sphereVAO);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLES, m_lightGeometries.sphereIndexCount, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	glDepthMask(GL_TRUE);
	
}

void LightingPass::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometries.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometries.fsqVAO);
	m_lightingProgram.DestroyHandle();
}

#pragma endregion