#include <Framework/DeferredRenderer.h>
#include <GL/glew.h>

#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <Framework/Object.h>
#include <Framework/Material.h>
#include <Framework/Light.h>

#include <imgui/imgui.h>
#include <iostream>

#define FRAMEBUFFER_WIDTH 800
#define FRAMEBUFFER_HEIGHT 600

#pragma region "Constructors/Destructor"

DeferredRenderer::DeferredRenderer() : m_deferredPassProgram(), m_defaultFramebuffer({ 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, GL_BACK_LEFT })
{
	m_gBuffer.framebuffer = 0;
	memset(m_gBuffer.colorBuffers, 0, sizeof(int) * 4);
}


DeferredRenderer::~DeferredRenderer()
{
}

bool DeferredRenderer::Initialize()
{
	//create programs
	m_deferredPassProgram.CreateHandle();
	m_deferredPassProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/DeferredPass.vert");
	m_deferredPassProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/DeferredPass.frag");
	m_deferredPassProgram.Link();

	m_lightingPassProgram.CreateHandle();
	m_lightingPassProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/LightingPass.vert");
	m_lightingPassProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/LightingPass.frag");
	m_lightingPassProgram.Link();

	//this data doesn't change, can set it now
	m_lightingPassProgram.SetUniform("uColor0", 1);
	m_lightingPassProgram.SetUniform("uColor1", 2);
	m_lightingPassProgram.SetUniform("uColor2", 3);
	m_lightingPassProgram.SetUniform("uColor3", 4);
	//will be updated when resized
	m_lightingPassProgram.SetUniform("uWindowSize", glm::vec2(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT));

	//generate g-buffer
	CreateGBuffer(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	//light geometries quad
	glGenVertexArrays(1, &m_lightGeometry.fsqVAO);
	glBindVertexArray(m_lightGeometry.fsqVAO);
	glGenBuffers(1, &m_lightGeometry.fsqVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lightGeometry.fsqVBO);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	return true;
}

void DeferredRenderer::RenderScene(Scene const & scene) const
{
//----------------------------------------------------------------------------------------------
//DEFERRED PASS
//----------------------------------------------------------------------------------------------

	//prepare for pass
	static std::vector<std::pair<Light const *, glm::vec3>> lights(100);
	lights.clear();

	BindGBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	m_deferredPassProgram.Use();

	//set per-frame uniforms
	m_deferredPassProgram.SetUniform("uProjectionMatrix", scene.GetProjectionMatrix());
	m_deferredPassProgram.SetUniform("uViewMatrix", scene.GetViewMatrix());

	//traverse the scene graph
	Node const * const & rootNode = GetRootNode(scene);
	RenderNode(rootNode, rootNode->GetTransformMatrix(), lights);

//----------------------------------------------------------------------------------------------
//LIGHTING PASS
//----------------------------------------------------------------------------------------------

	//prepare for pass
	BindDefaultFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);


	m_lightingPassProgram.Use();
	//set data per-frame uniforms
	m_lightingPassProgram.SetUniform("uEye", scene.GetCamera().GetPosition());

	for (auto lightPair : lights)
	{
		if (lightPair.first->IsGlobal())
		{
			//generate shadow map
		}

		m_lightingPassProgram.SetUniform("uLight.position", lightPair.second);
		m_lightingPassProgram.SetUniform("uLight.ambient", lightPair.first->GetAmbientIntensity());
		m_lightingPassProgram.SetUniform("uLight.intensity", lightPair.first->GetIntensity());

		glBindVertexArray(m_lightGeometry.fsqVAO);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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
	m_lightingPassProgram.SetUniform("uWindowSize", glm::vec2(width, height));
}

void DeferredRenderer::GenerateGUI()
{
	//ImGui::SetNextWindowSize(ImVec2(300, 300));
	ImGui::Begin("Positions");
	ImGui::Image((void *)(intptr_t)m_gBuffer.colorBuffers[0], ImVec2(300, 300), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(300, 300));
	ImGui::Begin("Normals");
	ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[1], ImVec2(300, 300), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(300, 300));
	ImGui::Begin("Kd");
	ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[2], ImVec2(300, 300), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();

	//ImGui::SetNextWindowSize(ImVec2(300, 300));
	ImGui::Begin("Ks/alpha");
	ImGui::Image((void*)(intptr_t)m_gBuffer.colorBuffers[3], ImVec2(300, 300), ImVec2(1, 1), ImVec2(0, 0));
	ImGui::End();
}

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

void DeferredRenderer::RenderNode(Node const * const & node, glm::mat4 modelMatrix, std::vector<std::pair<Light const *, glm::vec3>> & lights) const
{
	glm::mat4 cumulative = modelMatrix * node->GetTransformMatrix();
	if (node->GetNodeType() == Node::OBJECT_NODE)
	{
		Object const * object = static_cast<Object const *>(node);
		
		m_deferredPassProgram.SetUniform("uModelMatrix", modelMatrix * node->GetTransformMatrixWithScale());
		m_deferredPassProgram.SetUniform("uMaterial.kd", object->GetMaterial()->GetKd());
		m_deferredPassProgram.SetUniform("uMaterial.ks", object->GetMaterial()->GetKs());
		m_deferredPassProgram.SetUniform("uMaterial.alpha", object->GetMaterial()->GetAlpha());

		//render the object
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
	else if (node->GetNodeType() == Node::LIGHT_NODE)
	{
		//if we encounter a light, record it's world position
		//this information will be used in the lighting pass
		lights.push_back(std::make_pair(dynamic_cast<Light const *>(node), glm::vec3(cumulative[3][0], cumulative[3][1], cumulative[3][2])));
	}

	//render children
	for (auto childNode : IRenderer::GetChildren(node))
	{
		RenderNode(childNode, cumulative, lights);
	}

}

void DeferredRenderer::Finalize()
{
	glDeleteBuffers(1, &m_lightGeometry.fsqVBO);
	glDeleteVertexArrays(1, &m_lightGeometry.fsqVAO);

	FreeGBuffer();

	m_deferredPassProgram.DestroyHandle();
	m_lightingPassProgram.DestroyHandle();
}