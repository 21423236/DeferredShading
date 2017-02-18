#pragma once

#include <Framework/IRenderPass.h>
#include <Framework/Program.h>
#include <vector>

class GlobalLight;

class ShadowPass : public IRenderPass
{
public:
	
	//constructors/destructor
	ShadowPass(IRenderer const * renderer);
	~ShadowPass();

	//public methods
	void Initialize();
	void Prepare(Scene const & scene) const;
	void ProcessScene(Scene const & scene, std::vector<std::pair<GlobalLight const *,glm::vec3>> const & globalLights) const;
	void ProcessNode(Node const * const & node, glm::mat4 const & modelMatrix) const;
	void Finalize();

private:

	Program m_shadowProgram;

};

