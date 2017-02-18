#pragma once

#include <Framework/Program.h>
#include <vector>

class IRenderer;
class Scene;
class GlobalLight;
class LocalLight;

class LightingPass
{
public:

	//constructors/destructor
	LightingPass(IRenderer const * renderer);
	~LightingPass();

	//public methods
	void Initialize();
	void Prepare(Scene const & scene, glm::vec2 const & windowSize) const;
	void ProcessGlobalLights(std::vector<std::pair<GlobalLight const *,glm::vec3>> const & globalLights) const;
	void ProcessLocalLights(std::vector<std::pair<LocalLight const *, glm::vec3>> const & localLights) const;
	void Finalize();

protected:

	IRenderer const * m_renderer;

private:


	struct LightGeometries
	{
		unsigned int fsqVAO;
		unsigned int fsqVBO;
		unsigned int sphereVAO;
		unsigned int sphereVBO;
		unsigned int sphereIBO;
		unsigned int sphereIndexCount;
	} m_lightGeometries;
	
	Program m_globalLightProgram;
	Program m_localLightProgram;

};

