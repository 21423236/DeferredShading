#pragma once

#include <Framework/IRenderer.h>

class DeferredRenderer : public IRenderer
{
public:
	
	//constructors/destructor
	DeferredRenderer();
	~DeferredRenderer();

	//public methodsS
	bool Initialize();
	void Finalize();
	void RenderScene(Scene const & scene) const;

private:

};