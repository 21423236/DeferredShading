#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <Framework/Material.h>
#include <Framework/Object.h>
#include <Framework/Light.h>

#pragma region "Constructors/Destructor"

Scene::Scene() : m_rootNode(new Node()), m_camera()
{
}

Scene::~Scene()
{
	delete m_rootNode;

	for (auto mesh : m_meshes)
		delete mesh;
	for (auto material : m_materials)
		delete material;
	for (auto object : m_objects)
		delete object;
	for (auto light : m_lights)
		delete light;
}

#pragma endregion

#pragma region "Getters"

Camera & Scene::GetCamera()
{
	return m_camera;
}

Camera const & Scene::GetCamera() const
{
	return m_camera;
}

#pragma endregion

#pragma region "Public Methods"

Mesh * Scene::CreateMesh(char const * objPath)
{
	return nullptr;
}

Material * Scene::CreateMaterial(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha)
{
	Material * material = new Material(kd, ks, alpha);
	m_materials.push_back(material);
	return material;
}

Object * Scene::CreateObject()
{
	return nullptr;
}

Light * Scene::CreateLight()
{
	return nullptr;
}

#pragma endregion