#include <Framework/DeferredRenderer.h>
#include <GL/glew.h>

#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <Framework/Object.h>
#include <Framework/Material.h>
#include <Framework/Shape.h>
#include <Framework/LocalLight.h>

#include <imgui/imgui.h>
#include <iostream>

#include <Framework/Defaults.h>

#pragma region "Constructors/Destructor"

DeferredRenderer::DeferredRenderer() : m_defaultFramebuffer({ 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, GL_BACK_LEFT }), m_deferredPass(this), m_shadowPass(this), m_lightingPass(this)
{
	m_gBuffer.framebuffer = 0;
	memset(m_gBuffer.colorBuffers, 0, sizeof(int) * 4);
	m_gBuffer.depthBuffer = 0;
	m_gBuffer.width = DEFAULT_WINDOW_WIDTH;
	m_gBuffer.height = DEFAULT_WINDOW_HEIGHT;
	m_gBuffer.drawBuffers[0] = GL_COLOR_ATTACHMENT0;
	m_gBuffer.drawBuffers[1] = GL_COLOR_ATTACHMENT1;
	m_gBuffer.drawBuffers[2] = GL_COLOR_ATTACHMENT2;
	m_gBuffer.drawBuffers[3] = GL_COLOR_ATTACHMENT3;
}

DeferredRenderer::~DeferredRenderer()
{
}

#pragma endregion

#pragma region "Public Methods"

bool DeferredRenderer::Initialize()
{
	//generate framebuffers
	CreateGBuffer(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	CreateShadowBuffer(DEFAULT_SHADOW_WIDTH, DEFAULT_SHADOW_HEIGHT);
	
	m_debugProgram.CreateHandle();
	m_debugProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/DebugPass.vert");
	m_debugProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/DebugPass.frag");
	m_debugProgram.Link();

	//initialize passes
	m_deferredPass.Initialize();
	m_shadowPass.Initialize();
	m_lightingPass.Initialize();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{
	static std::vector<std::pair<GlobalLight const *, glm::vec3>> globalLights(100);
	static std::vector<std::pair<LocalLight const *, glm::vec3>> localLights(1000);
	static std::vector<Object const *> reflectiveObjects(1000);

	globalLights.clear();
	localLights.clear();
	reflectiveObjects.clear();

//-------------------------------------------------------------------------------------------------------
//DEFERRED PASS
//-------------------------------------------------------------------------------------------------------

	m_deferredPass.Prepare(scene);
	m_deferredPass.ProcessScene(scene, &globalLights, &localLights, nullptr);

//-------------------------------------------------------------------------------------------------------
//SHADOW MAP PASS
//-------------------------------------------------------------------------------------------------------

	m_shadowPass.Prepare(scene);
	m_shadowPass.ProcessScene(scene, globalLights);

//-------------------------------------------------------------------------------------------------------
//REFLECTION PASS
//-------------------------------------------------------------------------------------------------------

	//m_reflectionPass.Prepare(scene);
	//m_reflectionPass.ProcessScene(scene, reflectiveObjects);

//-------------------------------------------------------------------------------------------------------
//LIGHTING PASS
//-------------------------------------------------------------------------------------------------------

	//enable gamma correction

	m_lightingPass.Prepare(scene, glm::vec2(m_defaultFramebuffer.width, m_defaultFramebuffer.height));

	//enable gamme correction
	glEnable(GL_FRAMEBUFFER_SRGB);

	m_lightingPass.ProcessGlobalLights(globalLights);
	m_lightingPass.ProcessLocalLights(localLights);
	
	//disable gamma correction
	glDisable(GL_FRAMEBUFFER_SRGB);
	
//-------------------------------------------------------------------------------------------------------
//DEBUG PASS
//-------------------------------------------------------------------------------------------------------
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	
	m_debugProgram.Use();
	
	m_debugProgram.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_debugProgram.SetUniform("uViewMatrix", scene.GetViewMatrix());

	glBindVertexArray(Shape::GetWireCircle()->GetVAO());
	glEnableVertexAttribArray(0);
	
	for (auto const & lightPair : localLights)
	{
		m_debugProgram.SetUniform("uPosition", lightPair.second);
		m_debugProgram.SetUniform("uScale", lightPair.first->GetRadius());
		glDrawElements(GL_LINE_LOOP, Shape::GetWireCircle()->GetIndexCount(), GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void DeferredRenderer::Resize(int const & width, int const & height)
{
	m_defaultFramebuffer.width = width;
	m_defaultFramebuffer.height = height;
	FreeGBuffer();
	CreateGBuffer(width, height);
}

void DeferredRenderer::GenerateGUI()
{
	ImGui::Begin("Deferred Renderer");

	ImGui::Text("Intermediate Data:");
	ImGui::Indent(16.0f);

	if (ImGui::TreeNode("Positions"))
	{
		ImGui::Image((void *)(intptr_t)m_gBuffer.colorBuffers[0], ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Normals"))
	{
		ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[1], ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Kd"))
	{
		ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[2], ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Ks/alpha"))
	{
		ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[3], ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::TreePop();
	}

	ImGui::Unindent(16.0f);

	ImGui::End();
}

void DeferredRenderer::BindGBuffer() const
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

void DeferredRenderer::BindShadowBuffer(unsigned int const & shadowTexture) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowBuffer.framebuffer);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowTexture, 0);

	glDrawBuffers(1, &m_shadowBuffer.drawBuffers);
	glViewport(0, 0, m_shadowBuffer.width, m_shadowBuffer.height);
}

void DeferredRenderer::BlitDepthBuffers() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer.framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlitFramebuffer(0, 0, m_gBuffer.width, m_gBuffer.height, 0, 0, m_defaultFramebuffer.width, m_defaultFramebuffer.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void DeferredRenderer::Finalize()
{
	FreeGBuffer();
	FreeShadowBuffer();

	m_lightingPass.Finalize();
	m_shadowPass.Finalize();
	m_deferredPass.Finalize();
}

#pragma endregion

#pragma region "Private Methods"

void DeferredRenderer::CreateGBuffer(int const & width, int const & height)
{
	glGenFramebuffers(1, &m_gBuffer.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.framebuffer);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(4, m_gBuffer.colorBuffers);

	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBuffer.colorBuffers[0], 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBuffer.colorBuffers[1], 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBuffer.colorBuffers[2], 0);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_gBuffer.colorBuffers[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBuffer.colorBuffers[3], 0);

	glGenRenderbuffers(1, &m_gBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gBuffer.depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
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

void DeferredRenderer::CreateShadowBuffer(int const & width, int const & height)
{
	glGenFramebuffers(1, &m_shadowBuffer.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowBuffer.framebuffer);

	glGenRenderbuffers(1, &m_shadowBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_shadowBuffer.depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_shadowBuffer.depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shadowBuffer.width = width;
	m_shadowBuffer.height = height;

	m_shadowBuffer.drawBuffers = GL_COLOR_ATTACHMENT0;
}

void DeferredRenderer::FreeShadowBuffer()
{
	glDeleteRenderbuffers(1, &m_shadowBuffer.depthBuffer);
	glDeleteFramebuffers(1, &m_shadowBuffer.framebuffer);
}

#pragma endregion