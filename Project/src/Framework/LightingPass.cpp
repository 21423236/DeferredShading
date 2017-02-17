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
	m_lightingProgram.SetUniform("uShadow.shadowTexture", 5);

	//generate full screen quad
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

	//generate sphere
	struct Vertex
	{
		float x, y, z;
	};

	struct Face
	{
		int a, b, c, d;
	};

	std::vector<struct Vertex> sphereVertices;
	std::vector<struct Face> sphereIndices;
	float const PI = 3.14159f;
	float const rad = PI / 180.0f;
	int const n = 8;
	float const d = 2.0f * PI / (float)(n * 2);
	for (int i = 0; i < n * 2; ++i)
	{
		float s = i * 2.0f * PI / (float)(n * 2);
		for (int j = 0; j < n; ++j)
		{
			float t = j * PI / (float)n;
			float x = cos(s) * sin(t) * 0.5f;
			float y = sin(s) * sin(t) * 0.5f;
			float z = cos(t) * 0.5f;
			sphereVertices.push_back({ x, y, z });
			if (i > 0 && j > 0)
				sphereIndices.push_back({ (i - 1)*(n + 1) + (j - 1),
				(i - 1)*(n + 1) + (j),
					(i)*(n + 1) + (j),
					(i)*(n + 1) + (j - 1) });
		}
	}

	glGenVertexArrays(1, &m_lightGeometries.sphereVAO);
	glBindVertexArray(m_lightGeometries.sphereVAO);

	glGenBuffers(1, &m_lightGeometries.sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometries.sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct Vertex) * sphereVertices.size(), &sphereVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_lightGeometries.sphereIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lightGeometries.sphereIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(struct Face) * sphereIndices.size(), &sphereIndices[0], GL_STATIC_DRAW);
	m_lightGeometries.sphereIndexCount = sphereIndices.size();

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

	for (auto const & lightPair : globalLights)
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

	for (auto const & lightPair : localLights)
	{
		m_lightingProgram.SetUniform("uLight.position", lightPair.second);
		m_lightingProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_lightingProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		glBindVertexArray(m_lightGeometries.sphereVAO);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_QUADS, m_lightGeometries.sphereIndexCount, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
}

void LightingPass::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometries.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometries.fsqVAO);
	m_lightingProgram.DestroyHandle();
}

#pragma endregion