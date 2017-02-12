#include <Framework/DeferredRenderer.h>
#include <GL/glew.h>

#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <Framework/Object.h>

#define FRAMEBUFFER_WIDTH 800
#define FRAMEBUFFER_HEIGHT 600

#pragma region "Constructors/Destructor"

DeferredRenderer::DeferredRenderer() : m_program(), m_defaultFramebuffer({ 0, 800, 600, GL_BACK_LEFT })
{
	m_gBuffer.framebuffer = 0;
	memset(m_gBuffer.colorBuffers, 0, sizeof(int) * 4);
}


DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::Initialize()
{
	//create program
	m_program.CreateHandle();
	m_program.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/Basic.vert");
	m_program.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/Basic.frag");
	m_program.Link();

	//generate g-buffer
	CreateGBuffer(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	//light geometries quad
	glGenVertexArrays(1, &m_lightGeometry.fsqVAO);
	glBindVertexArray(m_lightGeometry.fsqVAO);
	glGenBuffers(1, &m_lightGeometry.fsqVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometry.fsqVBO);

	float fsqData[] = {
		-1.0f, 1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(fsqData), fsqData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(sizeof(float) * 2));

	glBindVertexArray(0);

	glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindDefaultFramebuffer();

	m_program.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_program.SetUniform("uViewMatrix", scene.GetViewMatrix());
	m_program.Use();
	
	Node const * const & rootNode = GetRootNode(scene);
	RenderNode(rootNode, rootNode->GetTransformMatrix());

}

void DeferredRenderer::Resize(int const & width, int const & height)
{
	m_defaultFramebuffer.width = width;
	m_defaultFramebuffer.height = height;
	FreeGBuffer();
	CreateGBuffer(width, height);
}

void DeferredRenderer::CreateGBuffer(int const & width, int const & height)
{
	glGenFramebuffers(1, &m_gBuffer.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.framebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(4, m_gBuffer.colorBuffers);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBuffer.colorBuffers[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBuffer.colorBuffers[1], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBuffer.colorBuffers[2], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBuffer.colorBuffers[3], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &m_gBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gBuffer.depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_gBuffer.width = width;
	m_gBuffer.height = height;

	m_gBuffer.drawBuffers[0] = GL_COLOR_ATTACHMENT0;
	m_gBuffer.drawBuffers[1] = GL_COLOR_ATTACHMENT1;
	m_gBuffer.drawBuffers[2] = GL_COLOR_ATTACHMENT2;
	m_gBuffer.drawBuffers[3] = GL_COLOR_ATTACHMENT3;

}

void DeferredRenderer::FreeGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &m_gBuffer.depthBuffer);
	glDeleteTextures(4, m_gBuffer.colorBuffers);
	glDeleteFramebuffers(1, &m_gBuffer.framebuffer);
}

void DeferredRenderer::BindGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.framebuffer);
	glDrawBuffers(4, m_gBuffer.drawBuffers);
	glViewport(0, 0, m_gBuffer.width, m_gBuffer.height);
}

void DeferredRenderer::BindDefaultFramebuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffers(1, &m_defaultFramebuffer.drawBuffers);
	glViewport(0, 0, m_defaultFramebuffer.width, m_defaultFramebuffer.height);
}

void DeferredRenderer::RenderNode(Node const * const & node, glm::mat4 modelMatrix) const
{
	glm::mat4 cumulative = modelMatrix * node->GetTransformMatrix();
	if (node->IsRenderable())
	{
		m_program.SetUniform("uModelMatrix", modelMatrix * node->GetTransformMatrixWithScale());
		Object const * object = dynamic_cast<Object const *>(node);
		glBindVertexArray(object->GetMesh()->GetVAO());
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glDrawArrays(GL_TRIANGLES, 0, object->GetMesh()->GetVertexCount());
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	for (auto childNode : IRenderer::GetChildren(node))
	{
		RenderNode(childNode, cumulative);
	}

}

void DeferredRenderer::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometry.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometry.fsqVAO);

	FreeGBuffer();

	m_program.DestroyHandle();
}