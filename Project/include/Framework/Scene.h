#pragma once

#include <Framework/Object.h>
#include <Framework/Camera.h>
#include <Framework/Program.h>

#include <glm/glm.hpp>

class Scene
{
public:

	//constructors/destructor
	Scene();
	~Scene();

	//getters
	Camera & GetCamera();
	Camera const & GetCamera() const;

private:

	//private methods
	
	Camera m_camera;
	Object m_rootObject;

};

