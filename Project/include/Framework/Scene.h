#pragma once

#include "Camera.h"
#include "Node.h"
#include <glm/glm.hpp>
#include <map>
#include <map>
#include <string>


class Mesh;
class Material;

class Scene
{

public:

	friend class IRenderer;

	//constructors/destructor
	Scene(unsigned const & windowWidth, unsigned const & windowHeight);
	~Scene();

	//getters
	Camera const & GetCamera() const;
	Camera & GetCamera();

	glm::mat4 const & GetProjectionMatrix() const;
	glm::mat4 const & GetViewMatrix() const;

	//setters
	void SetProjection(float const & ry, float const & front, float const & back);
	void SetWindowWidth(unsigned const & width);
	void SetWindowHeight(unsigned const & height);

	//public methods
	Mesh * CreateMesh(std::string const & name, std::string const & path);
	Material * CreateMaterial(std::string const & name, glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	void FreeMemory();

	void AddNode(Node * node);

	void Resize(int const & width, int const & height);

private:

	Node * m_rootNode;

	Camera m_camera;

	std::map<std::string, Mesh *> m_meshes;
	std::map<std::string, Material *> m_materials;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	unsigned m_windowWidth;
	unsigned m_windowHeight;

};

