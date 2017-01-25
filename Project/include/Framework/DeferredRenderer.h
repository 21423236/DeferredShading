#pragma once

#include <Framework/IRenderer.h>

class DeferredRenderer : public IRenderer
{
public:
	
	//constructors/destructor
	DeferredRenderer();
	~DeferredRenderer();

	//public methodsS
	void RenderScene(Scene const & scene) const;

};