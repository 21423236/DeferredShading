#pragma once

#include <Framework/Node.h>
#include <Framework/Mesh.h>
#include <Framework/Material.h>

class Mesh;

class Object : public Node
{
public:

	//Constructors/Destructor
	Object();
	~Object();

	//Getters
	Mesh const * const & GetMesh() const;
	Material const * const & GetMaterial() const;

	//Setters
	void SetMesh(Mesh const * const & mesh);

	//Public Methods

private:

	Mesh const * m_mesh;
	Material const * m_material;

};