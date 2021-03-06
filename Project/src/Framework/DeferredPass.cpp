#include <Framework/DeferredPass.h>
#include <Framework/DeferredRenderer.h>
#include <Framework/Scene.h>
#include <Framework/Object.h>
#include <Framework/GlobalLight.h>
#include <Framework/LocalLight.h>
#include <Framework/Material.h>
#include <Framework/Mesh.h>

#include <Framework/Defaults.h>

#include <GL/glew.h>

#pragma region "Constructors/Destructor"

DeferredPass::DeferredPass(IRenderer const * renderer) : IRenderPass(renderer), m_globalLights(nullptr), m_localLights(nullptr), m_reflectiveObjects(nullptr), m_deferredProgram()
{
}

DeferredPass::~DeferredPass()
{
}

#pragma endregion

#pragma region "Public Methods"

void DeferredPass::Initialize()
{
	m_deferredProgram.CreateHandle();
	m_deferredProgram.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/DeferredPass.vert");
	m_deferredProgram.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/DeferredPass.frag");
	m_deferredProgram.Link();

	m_deferredProgram.SetUniform("uMaterial.diffuseMap", 9);
	m_deferredProgram.SetUniform("uMaterial.normalMap", 10);
	m_deferredProgram.SetUniform("uMaterial.specularMap", 11);
}

void DeferredPass::Prepare(Scene const & scene) const
{
	//bind g-buffer;
	dynamic_cast<DeferredRenderer const *>(m_renderer)->BindGBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	m_deferredProgram.Use();
}

void DeferredPass::ProcessScene(Scene const & scene, std::vector<std::pair<GlobalLight const *, glm::vec3>> * globalLights, std::vector<struct LocalLightInformation> * localLights, std::vector<Object const *> * reflectiveObjects) const
{
	m_globalLights = globalLights;
	m_localLights = localLights;
	m_reflectiveObjects = reflectiveObjects;
	scene.Traverse(*this);
}

void DeferredPass::ProcessNode(Node const * const & node, glm::mat4 const & modelMatrix) const
{
	if (node->GetNodeType() == Node::OBJECT_NODE)
	{
		Object const * object = dynamic_cast<Object const *>(node);

		/*
		//if reflective, collect it
		if(object->IsReflective())
			m_reflectiveObjects->push_back(object);
		*/

		m_deferredProgram.SetUniform("uModelMatrix", modelMatrix);
		m_deferredProgram.SetUniform("uMaterial.kd", object->GetMaterial()->GetKd());
		m_deferredProgram.SetUniform("uMaterial.ks", object->GetMaterial()->GetKs());
		m_deferredProgram.SetUniform("uMaterial.alpha", object->GetMaterial()->GetAlpha());

		if (object->GetMaterial()->HasDiffuseMap())
		{
			m_deferredProgram.SetUniform("uMaterial.hasDiffuseMap", true);
			glActiveTexture(DIFFUSE_MAP_TEXTURE_UNIT);
			glBindTexture(GL_TEXTURE_2D, object->GetMaterial()->GetDiffuseMap()->GetHandle());
		}
		else
			m_deferredProgram.SetUniform("uMaterial.hasDiffuseMap", false);

		if (object->GetMaterial()->HasNormalMap())
		{
			m_deferredProgram.SetUniform("uMaterial.hasNormalMap", true);
			glActiveTexture(NORMAL_MAP_TEXTURE_UNIT);
			glBindTexture(GL_TEXTURE_2D, object->GetMaterial()->GetNormalMap()->GetHandle());
		}
		else
			m_deferredProgram.SetUniform("uMaterial.hasNormalMap", false);

		if (object->GetMaterial()->HasSpecularMap())
		{
			m_deferredProgram.SetUniform("uMaterial.hasSpecularMap", true);
			glActiveTexture(SPECULAR_MAP_TEXTURE_UNIT);
			glBindTexture(GL_TEXTURE_2D, object->GetMaterial()->GetSpecularMap()->GetHandle());
		}
		else
			m_deferredProgram.SetUniform("uMaterial.hasSpecularMap", false);


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

	}
	else if (node->GetNodeType() == Node::GLOBAL_LIGHT_NODE)
	{
		GlobalLight const * light = dynamic_cast<GlobalLight const *>(node);
		glm::vec3 position(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
		m_globalLights->push_back(std::make_pair(light, position));
			
	}
	else if (node->GetNodeType() == Node::LOCAL_LIGHT_NODE)
	{
		LocalLight const * light = dynamic_cast<LocalLight const*>(node);
		glm::vec3 position(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
		glm::vec3 const & intensity = light->GetIntensity();
		m_localLights->push_back({ {position.x, position.y, position.z, 0.0f}, {intensity.x, intensity.y, intensity.z}, light->GetRadius() });
	}
}

void DeferredPass::Finalize()
{
	m_deferredProgram.DestroyHandle();
}

#pragma endregion

#pragma region "Getters"

Program const & DeferredPass::GetProgram() const
{
	return m_deferredProgram;
}

#pragma endregion