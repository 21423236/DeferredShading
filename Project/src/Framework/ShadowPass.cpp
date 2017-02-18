#include <Framework/ShadowPass.h>
#include <Framework/Scene.h>
#include <Framework/GlobalLight.h>
#include <Framework/Object.h>
#include <Framework/Mesh.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Defaults.h>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#pragma region "Constructors/Destructor"

ShadowPass::ShadowPass(IRenderer const * renderer) : IRenderPass(renderer)
{
}


ShadowPass::~ShadowPass()
{
}

#pragma endregion

#pragma region "Public Methods"

static float const front = 0.1f;
static float const back = 100.0f;
static glm::mat4 const g_projectionMatrix(glm::vec4(5, 0, 0, 0),
											glm::vec4(0, 1.0f / (0.2f * ((float)DEFAULT_SHADOW_WIDTH / (float)DEFAULT_SHADOW_HEIGHT)), 0, 0),
											glm::vec4(0, 0, (front - back) / (back - front), -1.0f),
											glm::vec4(0, 0, (-2.0f * front * back) / (back - front), 0.0f));

static glm::mat4 const g_BMatrix(glm::vec4(0.5, 0, 0, 0), glm::vec4(0, 0.5, 0, 0), glm::vec4(0, 0, 0.5, 0), glm::vec4(0.5, 0.5, 0.5, 1));

void ShadowPass::Initialize()
{
	m_shadowProgram.CreateHandle();
	m_shadowProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/ShadowPass.vert");
	m_shadowProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/ShadowPass.frag");
	m_shadowProgram.Link();
}

void ShadowPass::Prepare(Scene const & scene) const
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	m_shadowProgram.Use();
}

void ShadowPass::ProcessScene(Scene const & scene, std::vector<std::pair<GlobalLight const *, glm::vec3>> const & globalLights) const
{
	for (auto const & lightPair : globalLights)
	{
		//bind shadow framebuffer
		dynamic_cast<DeferredRenderer const *>(m_renderer)->BindShadowBuffer(lightPair.first->GetShadowMap());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 shadowMatrix = g_projectionMatrix * glm::lookAt(lightPair.second, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		m_shadowProgram.SetUniform("uShadowMatrix", shadowMatrix);
		lightPair.first->m_shadowMatrix = g_BMatrix * shadowMatrix;

		//traverse the scene
		scene.Traverse(*this);
	}
}

void ShadowPass::ProcessNode(Node const * const & node, glm::mat4 const & modelMatrix) const
{
	if (node->GetNodeType() == Node::OBJECT_NODE)
	{
		m_shadowProgram.SetUniform("uModelMatrix", modelMatrix);
		Object const * object = dynamic_cast<Object const *>(node);
		glBindVertexArray(object->GetMesh()->GetVAO());
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, object->GetMesh()->GetVertexCount());
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
}

void ShadowPass::Finalize()
{

}

#pragma endregion