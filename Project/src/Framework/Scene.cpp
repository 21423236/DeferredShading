#include <Framework/Scene.h>

#pragma region "Constructors/Destructor"

Scene::Scene() : m_camera(), m_rootObject()
{
}

Scene::~Scene()
{
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

#pragma region "Setters"

#pragma endregion