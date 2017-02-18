#include <Framework/LightingPass.h>
#include <Framework/Program.h>
#include <Framework/Light.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Scene.h>

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

	glm::vec3 eyePosition = glm::vec3(glm::inverse(scene.GetViewMatrix()) * glm::vec4(0, 0, 0, 1));

	m_globalLightProgram.SetUniform("uWindowSize", windowSize);
	m_globalLightProgram.SetUniform("uEye", eyePosition);

	m_localLightProgram.SetUniform("uWindowSize", windowSize);
	m_localLightProgram.SetUniform("uEye", eyePosition);
	m_localLightProgram.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_localLightProgram.SetUniform("uViewMatrix", scene.GetViewMatrix());
}

void LightingPass::ProcessGlobalLights(std::vector<std::pair<Light const *,glm::vec3>> const & globalLights) const
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	m_globalLightProgram.Use();

	for (auto const & lightPair : globalLights)
	{
		m_globalLightProgram.SetUniform("uLight.position", lightPair.second);
		m_globalLightProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_globalLightProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		m_globalLightProgram.SetUniform("uShadow.matrix", lightPair.first->GetShadowMatrix());
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
	//glEnable(GL_DEPTH_TEST);

	m_localLightProgram.Use();

	for (auto const & lightPair : localLights)
	{
		m_localLightProgram.SetUniform("uLight.position", lightPair.second);
		m_localLightProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_localLightProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());
		m_localLightProgram.SetUniform("uLight.radius", lightPair.first->GetRadius());

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
	m_localLightProgram.DestroyHandle();
	m_globalLightProgram.DestroyHandle();
}

#pragma endregion