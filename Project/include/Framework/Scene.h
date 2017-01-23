#pragma once

#include <Framework/Object.h>
#include <Framework/Camera.h>
#include <Framework/Program.h>

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
	void RenderScene() const;

private:
	
	Camera m_camera;
	Object m_rootObject;

};

