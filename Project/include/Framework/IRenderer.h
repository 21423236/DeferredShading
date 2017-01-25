#pragma once

class Scene;

class IRenderer
{
public:
	virtual ~IRenderer() {}
	virtual bool Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void RenderScene(Scene const & scene) const = 0;
};