#pragma once

#include "Camera.h"
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <string>


class Object;
class Mesh;

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
	Object * LoadObject(std::string const & path);
	void FreeMemory();


	Mesh * testMesh;

private:

	Camera m_camera;

	std::vector<Object *> m_objects;
	std::vector<Mesh *> m_meshes;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;

	unsigned m_windowWidth;
	unsigned m_windowHeight;

};

