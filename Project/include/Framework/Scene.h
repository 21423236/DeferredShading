#pragma once

#include <Framework/Camera.h>
#include <glm/glm.hpp>
#include <vector>

class Node;
class Mesh;
class Material;
class Object;
class Light;

class Scene
{

public:

	//constructors/destructor
	Scene();
	~Scene();

	//getters
	Camera & GetCamera();
	Camera const & GetCamera() const;

	//public methods
	Mesh * CreateMesh(char const * objPath);
	Material * CreateMaterial(glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	Object * CreateObject();
	Light * CreateLight();


private:

	Node * m_rootNode;

	Camera m_camera;

	std::vector<Mesh *> m_meshes;
	std::vector<Material *> m_materials;
	std::vector<Object *> m_objects;
	std::vector<Light *> m_lights;

};

