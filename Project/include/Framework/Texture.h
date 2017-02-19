#pragma once

class Texture
{
public:

	friend class DeferredRenderer;
	friend class GUI;
	
	typedef enum DebugCorrection
	{
		NONE,
		POSITIONS,
		NORMALS,
		DEPTH
	} DebugCorrectionType;

	//constructors/destructor
	Texture(unsigned int unit = 6, DebugCorrectionType correction = NONE);
	~Texture();

	//public methods
	void Initialize(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type, void * pixels);
	void Bind() const;
	void Free();

private:

	unsigned int m_handle;
	unsigned int m_unit;
	DebugCorrectionType m_correction;

};

