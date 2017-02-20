#pragma once

#include "Camera.h"
#include "Node.h"
#include <glm/glm.hpp>
#include <map>
#include <map>
#include <string>


class Mesh;
class Material;
class IRenderPass;

class Scene
{

public:

	friend class IRenderer;
	friend class GUI;

	//constructors/destructor
	Scene(unsigned const & windowWidth, unsigned const & windowHeight);
	~Scene();

	//getters
	Camera const & GetCamera() const;
	Camera & GetCamera();
	glm::mat4 const & GetProjectionMatrix() const;
	glm::mat4 const & GetViewMatrix() const;
	glm::vec3 const & GetSceneSize() const;
	glm::vec3 const & GetAmbientIntensity() const;

	//setters
	void SetProjection(float const & ry, float const & front, float const & back);
	void SetSceneSize(glm::vec3 const & sceneSize);
	void SetAmbientIntensity(glm::vec3 const & ambientIntensity);
	void SetWindowWidth(unsigned const & width);
	void SetWindowHeight(unsigned const & height);

	//public methods
	Mesh * CreateMesh(std::string const & name, std::string const & path);
	Material * CreateMaterial(std::string const & name, glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	void AddNode(Node * node);
	void Traverse(IRenderPass const & pass) const;
	void Resize(int const & width, int const & height);
	void FreeMemory();

private:

	//private methods
	void TraverseNode(Node * const & node, IRenderPass const & pass, glm::mat4 const & modelMatrix) const;

	Node * m_rootNode;

	Camera m_camera;

	std::map<std::string, Mesh *> m_meshes;
	std::map<std::string, Material *> m_materials;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::vec3 m_sceneSize;
	glm::vec3 m_ambientIntensity;

	unsigned m_windowWidth;
	unsigned m_windowHeight;

};

