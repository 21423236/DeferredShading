#pragma once

class Scene;

class IRenderer
{
public:
	virtual ~IRenderer() {}
	virtual void RenderScene(Scene const & scene) const = 0;
};