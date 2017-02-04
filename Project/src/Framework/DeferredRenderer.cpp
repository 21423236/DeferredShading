#include <Framework/DeferredRenderer.h>
#include <GL/glew.h>

#define FRAMEBUFFER_WIDTH 800
#define FRAMEBUFFER_HEIGHT 600

#pragma region "Constructors/Destructor"

DeferredRenderer::DeferredRenderer() : m_gBuffer({0, 0, 0, 0, 0, 0}), m_program()
{
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
	glGenFramebuffers(1, &m_gBuffer.famebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.famebuffer);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(4, m_gBuffer.colorBuffers);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBuffer.colorBuffers[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBuffer.colorBuffers[1], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBuffer.colorBuffers[2], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBuffer.colorBuffers[3], 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &m_gBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gBuffer.depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

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
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_program.Use();
	glBindVertexArray(m_lightGeometry.fsqVAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void DeferredRenderer::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometry.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometry.fsqVAO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteRenderbuffers(1, &m_gBuffer.depthBuffer);
	glDeleteTextures(4, m_gBuffer.colorBuffers);
	glDeleteFramebuffers(1, &m_gBuffer.famebuffer);

	m_program.DestroyHandle();
}