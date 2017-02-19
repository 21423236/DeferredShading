#pragma once

class Texture
{
public:

	friend class DeferredRenderer;
	friend class GUI;
	
	typedef enum DebugCorrection
	{
		NONE = 0,
		POSITIONS = 1,
		NORMALS = 2,
		DEPTH = 3
	} DebugCorrectionType;

	//constructors/destructor
	Texture(unsigned int unit = 6, DebugCorrectionType correction = NONE);
	~Texture();

	//public methods
	void Initialize(unsigned int width, unsigned int height, unsigned int internalFormat, unsigned int format, unsigned int type, void * pixels);
	void Bind() const;
	void Free();

	//getters
	unsigned int const & GetHandle() const;
	DebugCorrectionType const & GetCorrectionType() const;

private:

	unsigned int m_handle;
	unsigned int m_unit;
	DebugCorrectionType m_correction;

};

