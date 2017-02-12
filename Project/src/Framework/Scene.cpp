#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma region "Constructors/Destructor"

Scene::Scene(unsigned const & windowWidth, unsigned const & windowHeight) : m_camera(m_viewMatrix), m_meshes(), m_projectionMatrix(), m_viewMatrix(), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{

}

Scene::~Scene()
{

}

#pragma endregion

#pragma region "Getters"

Camera const & Scene::GetCamera() const
{
	return m_camera;
}

Camera & Scene::GetCamera()
{
	return m_camera;
} 

glm::mat4 const & Scene::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

glm::mat4 const & Scene::GetViewMatrix() const
{
	return m_viewMatrix;
}

#pragma endregion

#pragma region "Setters"

void Scene::SetProjection(float const & ry, float const & front, float const & back)
{
	float rx = ry * (float)m_windowWidth / (float)m_windowHeight;
	m_projectionMatrix = glm::mat4(
		glm::vec4(1.0f / rx, 0.0f, 0.0f, 0.0f), 
		glm::vec4(0.0f, 1.0f / ry, 0.0f, 0.0f), 
		glm::vec4(0.0f, 0.0f, (front - back) / (back - front), -1.0f), 
		glm::vec4(0.0f, 0.0f, (-2.0f * front * back) / (back - front), 0.0f));
}

void Scene::SetWindowWidth(unsigned const & width)
{
	m_windowWidth = width;
}

void Scene::SetWindowHeight(unsigned const & height)
{
	m_windowHeight = height;
}

#pragma endregion

#pragma region "Public Methods"

Mesh * Scene::LoadMesh(std::string const & path)
{
	Assimp::Importer importer;
	aiScene const * scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate);

	if (!scene || !scene->HasMeshes())
		return nullptr;

	//process the object
	aiMesh * mesh = scene->mMeshes[0];
	Mesh * gpuMesh = new Mesh(mesh->mNumFaces, mesh->mFaces, mesh->mVertices, mesh->mNormals, mesh->mTangents, mesh->mTextureCoords);
	testMesh = gpuMesh;
	m_meshes.push_back(gpuMesh);

	return gpuMesh;
}

void Scene::FreeMemory()
{
	for (auto mesh : m_meshes)
		delete mesh;
}

#pragma endregion