#pragma once

#include "IRenderPass.h"
#include "Program.h"
#include "LocalLight.h"

#include <vector>

class Object;
class GlobalLight;
class LocalLight;


class DeferredPass : public IRenderPass
{
public:

	//constructors/destructor
	DeferredPass(IRenderer const * renderer);
	~DeferredPass();

	//public methods
	void Initialize();
	void Prepare(Scene const & scene) const;
	void ProcessScene(Scene const & scene, std::vector<std::pair<GlobalLight const *, glm::vec3>> * globalLights, std::vector<struct LocalLightInformation> * localLights, std::vector<Object const *> * reflectiveObjects) const;
	void ProcessNode(Node const * const & node, glm::mat4 const & modelMatrix) const;
	void Finalize();

	//getters
	Program const & GetProgram() const;

private:

	//private state
	mutable std::vector<std::pair<GlobalLight const *, glm::vec3>> * m_globalLights;
	mutable std::vector<struct LocalLightInformation> * m_localLights;
	mutable std::vector<Object const *> * m_reflectiveObjects;

	Program m_deferredProgram;

};

