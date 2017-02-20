#pragma once

#include <Framework/Program.h>
#include <vector>

class IRenderer;
class Scene;
class GlobalLight;
class LocalLight;
class Shape;

class LightingPass
{
public:

	//constructors/destructor
	LightingPass(IRenderer const * renderer);
	~LightingPass();

	//public methods
	void Initialize();
	void Prepare(Scene const & scene) const;
	void ProcessAmbientLight() const;
	void ProcessGlobalLights(std::vector<std::pair<GlobalLight const *,glm::vec3>> const & globalLights) const;
	void ProcessLocalLights(unsigned int const & lightsCount) const;
	void Finalize();

protected:

	IRenderer const * m_renderer;

private:
	
	Program m_ambientLightProgram;
	Program m_globalLightProgram;
	Program m_localLightProgram;

};

