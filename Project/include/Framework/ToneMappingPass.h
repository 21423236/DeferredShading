#pragma once

#include "Program.h"

class IRenderer;

class ToneMappingPass
{
public:

	friend class DeferredRenderer;

	//constructors/destructor
	ToneMappingPass(IRenderer const * renderer);
	~ToneMappingPass();

	//public methods
	void Initialize();
	void Prepare() const;
	void ProcessFrame() const;
	void Finalize();

private:

	IRenderer const * m_renderer;
	Program m_toneMappingProgram;

	typedef enum ToneMappingMethod
	{
		REINHARD = 0,
		EXPOSURE = 1
	} ToneMappingMethod;
	
	ToneMappingMethod m_method;
	float m_gamma;
	float m_exposure;

};

