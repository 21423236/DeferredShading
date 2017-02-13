#pragma once

#include "Node.h"

class Mesh;
class Material;

class Object : public Node
{
public:

	//constructors/destructor
	Object(Mesh * mesh, Material * material);
	~Object();

	//getters
	Mesh const * const & GetMesh() const;
	Material const * const & GetMaterial() const;

	NodeType GetNodeType() const;


private:

	Mesh * m_mesh;
	Material * m_material;

};

