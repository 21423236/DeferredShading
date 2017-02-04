#pragma once

class Scene;
class Object;
class Mesh;
class Material;

class IRenderer
{
public:

	virtual ~IRenderer() {}
	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void RenderScene(Scene const & scene) const = 0;

};