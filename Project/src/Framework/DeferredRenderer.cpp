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

DeferredRenderer::DeferredRenderer() : m_gBuffer({ 0, Texture(GBUFFER_COLOR_BUFFER0_UNIT, Texture::POSITIONS), 
														Texture(GBUFFER_COLOR_BUFFER1_UNIT, Texture::NORMALS), 
														Texture(GBUFFER_COLOR_BUFFER2_UNIT), 
														Texture(GBUFFER_COLOR_BUFFER3_UNIT),
														Texture(GBUFFER_DEPTH_BUFFER_UNIT), 0, 0, {0, 0, 0, 0} }), 
										m_shadowBuffer({ 0, 0, 0, 0, 0 }), 
										m_lightAccumulationBuffer({0, Texture(LIGHT_ACCUMULATION_BUFFER_UNIT), 0, 0, 0}),
										m_defaultFramebuffer({ 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, GL_BACK_LEFT }), 
										m_sceneUniformBuffer(0), 
										m_localLightsBuffer(1, 1000), 
										m_debugProgram(), 
										m_deferredPass(this), 
										m_shadowPass(this), 
										m_lightingPass(this),
										m_toneMappingPass(this),
										m_gatherStatistics(false), 
										m_displayLightVolumes(false)
{
	
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
	CreateLightAccumulationBuffer(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	
	m_debugProgram.CreateHandle();
	m_debugProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/DebugPass.vert");
	m_debugProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/DebugPass.frag");
	m_debugProgram.Link();

	//initialize uniform buffer
	m_sceneUniformBuffer.AddUniform("uScene.ProjectionMatrix", GL_FLOAT_MAT4);
	m_sceneUniformBuffer.AddUniform("uScene.ViewMatrix", GL_FLOAT_MAT4);
	m_sceneUniformBuffer.AddUniform("uScene.WindowSize", GL_FLOAT_VEC2);
	m_sceneUniformBuffer.AddUniform("uScene.SceneSize", GL_FLOAT_VEC2);
	m_sceneUniformBuffer.AddUniform("uScene.EyePosition", GL_FLOAT_VEC3);
	m_sceneUniformBuffer.Initialize();

	//initialize local lights buffer
	m_localLightsBuffer.Initialize();

	//initialize passes
	m_deferredPass.Initialize();
	m_shadowPass.Initialize();
	m_lightingPass.Initialize();
	m_toneMappingPass.Initialize();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_CULL_FACE);
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{
	static std::vector<std::pair<GlobalLight const *, glm::vec3>> globalLights(100);
	static std::vector<Object const *> reflectiveObjects(1000);

	globalLights.clear();
	m_localLightsBuffer.m_buffer.clear();
	reflectiveObjects.clear();

	//upload global uniform data
	m_sceneUniformBuffer.SetUniform("uScene.ProjectionMatrix", scene.GetProjectionMatrix());
	m_sceneUniformBuffer.SetUniform("uScene.ViewMatrix", scene.GetViewMatrix());
	m_sceneUniformBuffer.SetUniform("uScene.WindowSize", glm::vec2(m_defaultFramebuffer.width, m_defaultFramebuffer.height));
	m_sceneUniformBuffer.SetUniform("uScene.SceneSize", scene.GetSceneSize());
	m_sceneUniformBuffer.SetUniform("uScene.EyePosition", glm::vec3(glm::inverse(scene.GetViewMatrix()) * glm::vec4(0, 0, 0, 1)));
	m_sceneUniformBuffer.UploadBuffer();

//-------------------------------------------------------------------------------------------------------
//DEFERRED PASS
//-------------------------------------------------------------------------------------------------------

	m_deferredPass.Prepare(scene);
	m_deferredPass.ProcessScene(scene, &globalLights, &m_localLightsBuffer.m_buffer, nullptr);

	//upload local light information
	m_localLightsBuffer.Upload();

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

	m_lightingPass.Prepare(scene);

	m_lightingPass.ProcessAmbientLight();
	m_lightingPass.ProcessGlobalLights(globalLights);
	m_lightingPass.ProcessLocalLights(m_localLightsBuffer.m_buffer.size());
	
	
//-------------------------------------------------------------------------------------------------------
//FORWARD PASS (skydome, transparent objects)
//-------------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------------
//GAMMA CORRECTION\TONE MAPPING PASS
//-------------------------------------------------------------------------------------------------------

	m_toneMappingPass.Prepare();
	m_toneMappingPass.ProcessFrame();

//-------------------------------------------------------------------------------------------------------
//DEBUG DRAWING
//-------------------------------------------------------------------------------------------------------
	
	if (m_displayLightVolumes)
	{
		BlitDepthBuffers();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		m_debugProgram.Use();

		glBindVertexArray(Shape::GetWireCircle()->GetVAO());
		glEnableVertexAttribArray(0);

		glDrawElementsInstanced(GL_LINE_LOOP, Shape::GetWireCircle()->GetIndexCount(), GL_UNSIGNED_INT, 0, m_localLightsBuffer.m_buffer.size());

		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}
}

void DeferredRenderer::Resize(int const & width, int const & height)
{
	m_defaultFramebuffer.width = width;
	m_defaultFramebuffer.height = height;
	FreeGBuffer();
	CreateGBuffer(width, height);
	FreeLightAccumulationBuffer();
	CreateLightAccumulationBuffer(width, height);
}

void DeferredRenderer::GenerateGUI()
{
	if (!ImGui::Begin("Deferred Renderer", 0, ImGuiWindowFlags_ShowBorders))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Debug Options"))
	{
		ImGui::Checkbox("Gather Statistics", &m_gatherStatistics);
		ImGui::Checkbox("Display Light Volumes", &m_displayLightVolumes);
	}

	if (ImGui::CollapsingHeader("Geometry Pass"))
	{
		ImGui::Text("Statistics:");
		ImGui::Text("Elapsed Time:");
		ImGui::SameLine();
		
		if (m_gatherStatistics)
			ImGui::Text("time");
		else
			ImGui::Text("N/A");

		ImGui::Separator();
		
		ImGui::Text("Intermediate Results:");
		if (ImGui::TreeNode("Positions"))
		{
			ImGui::Image((void*)&m_gBuffer.colorBuffer0, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Format: (P.x, P.y, P.z, P.w)");
				ImGui::Text("Size: %i x %i", m_gBuffer.colorBuffer0.m_width, m_gBuffer.colorBuffer0.m_height);
				ImGui::EndTooltip();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Normals"))
		{
			ImGui::Image((void*)&m_gBuffer.colorBuffer1, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Format: (N.x, N.y, N.z, 1)");
				ImGui::Text("Size: %i x %i", m_gBuffer.colorBuffer1.m_width, m_gBuffer.colorBuffer1.m_height);
				ImGui::EndTooltip();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Kd"))
		{
			ImGui::Image((void*)&m_gBuffer.colorBuffer2, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Format: (Kd.r, Kd.g, Kd.b, 1");
				ImGui::Text("Size: %i x %i", m_gBuffer.colorBuffer2.m_width, m_gBuffer.colorBuffer3.m_height);
				ImGui::EndTooltip();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Ks/alpha"))
		{
			ImGui::Image((void*)&m_gBuffer.colorBuffer3, ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Format: (Ks.r, Ks.g, Ks.b, alpha)");
				ImGui::Text("Size: %i x %i", m_gBuffer.colorBuffer3.m_width, m_gBuffer.colorBuffer3.m_height);
				ImGui::EndTooltip();
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Shadow Pass"))
	{
		ImGui::Text("Statistics:");
		ImGui::Spacing();
		ImGui::Text("Elapsed Time:");
		ImGui::SameLine();

		if (m_gatherStatistics)
			ImGui::Text("time");
		else
			ImGui::Text("N/A");

		ImGui::Text("Lights Processed: %i", m_shadowPass.GetNumberOfProcessedLights());

		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Lighting Pass"))
	{
		ImGui::Text("Statistics:");
		ImGui::Text("Elapsed Time:");
		ImGui::SameLine();

		if (m_gatherStatistics)
			ImGui::Text("time");
		else
			ImGui::Text("N/A");

		ImGui::Text("Global Lights: %i", m_lightingPass.GetGlobalLightsCount());
		ImGui::Text("Local Lights: %i", m_lightingPass.GetLocalLightsCount());
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Forward Pass"))
	{
		ImGui::Text("Statistics:");
		ImGui::Text("Elapsed Time:");
		ImGui::SameLine();

		if (m_gatherStatistics)
			ImGui::Text("time");
		else
			ImGui::Text("N/A");

		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Tone Mapping"))
	{
		ImGui::DragFloat("Gamma", &m_toneMappingPass.m_gamma, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Expsoure", &m_toneMappingPass.m_exposure, 0.01f, 0.0f, 10.0f);
	}

	ImGui::End();
	
}

void DeferredRenderer::BindGBuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer.framebuffer);
	glDrawBuffers(4, m_gBuffer.drawBuffers);
	glViewport(0, 0, m_gBuffer.width, m_gBuffer.height);
}

void DeferredRenderer::BindShadowBuffer(Texture const & shadowTexture) const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowBuffer.framebuffer);
	shadowTexture.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowTexture.m_handle, 0);

	glDrawBuffers(1, &m_shadowBuffer.drawBuffers);
	glViewport(0, 0, m_shadowBuffer.width, m_shadowBuffer.height);
}

void DeferredRenderer::BindLightAccumulationBuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightAccumulationBuffer.framebuffer);
	glDrawBuffers(1, &m_lightAccumulationBuffer.drawBuffers);
	glViewport(0, 0, m_lightAccumulationBuffer.width, m_lightAccumulationBuffer.height);
}

void DeferredRenderer::BindDefaultFramebuffer() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffers(1, &m_defaultFramebuffer.drawBuffers);
	glViewport(0, 0, m_defaultFramebuffer.width, m_defaultFramebuffer.height);
}

void DeferredRenderer::BlitDepthBuffers() const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer.framebuffer);
	glBlitFramebuffer(0, 0, m_gBuffer.width, m_gBuffer.height, 0, 0, m_defaultFramebuffer.width, m_defaultFramebuffer.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void DeferredRenderer::Finalize()
{
	FreeGBuffer();
	FreeShadowBuffer();

	m_localLightsBuffer.Free();

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

	m_gBuffer.colorBuffer0.Initialize(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBuffer.colorBuffer0.m_handle, 0);
	m_gBuffer.colorBuffer1.Initialize(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBuffer.colorBuffer1.m_handle, 0);
	m_gBuffer.colorBuffer2.Initialize(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBuffer.colorBuffer2.m_handle, 0);
	m_gBuffer.colorBuffer3.Initialize(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBuffer.colorBuffer3.m_handle, 0);

	m_gBuffer.depthBuffer.Initialize(width, height, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBuffer.depthBuffer.GetHandle(), 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		int blah = 45;
	}

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
	m_gBuffer.depthBuffer.Free();
	m_gBuffer.colorBuffer0.Free();
	m_gBuffer.colorBuffer1.Free();
	m_gBuffer.colorBuffer2.Free();
	m_gBuffer.colorBuffer3.Free();
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

void DeferredRenderer::CreateLightAccumulationBuffer(int const & width, int const & height)
{
	glGenFramebuffers(1, &m_lightAccumulationBuffer.framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightAccumulationBuffer.framebuffer);

	//attach color buffer
	m_lightAccumulationBuffer.colorBuffer.Initialize(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightAccumulationBuffer.colorBuffer.GetHandle(), 0);

	//attach depth buffer
	m_gBuffer.depthBuffer.Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBuffer.depthBuffer.GetHandle(), 0);

	m_lightAccumulationBuffer.width = width;
	m_lightAccumulationBuffer.height = height;

	m_lightAccumulationBuffer.drawBuffers = GL_COLOR_ATTACHMENT0;
}

void DeferredRenderer::FreeLightAccumulationBuffer()
{
	glDeleteFramebuffers(1, &m_lightAccumulationBuffer.framebuffer);
	m_lightAccumulationBuffer.colorBuffer.Free();
}

#pragma endregion