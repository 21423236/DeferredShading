#include <Framework/Object.h>

#pragma region "Constructors/Destructor"

Object::Object(Mesh * mesh, Material * material) : Node(), m_mesh(mesh), m_material(material)
{
}

Object::~Object()
{
}

#pragma endregion

#pragma region "Getters"

Mesh const * const & Object::GetMesh() const
{
	return m_mesh;
}

Material const * const & Object::GetMaterial() const
{
	return m_material;
}

#pragma endregion

#pragma region "Public Methods"

Node::NodeType Object::GetNodeType() const
{
	return OBJECT_NODE;
}

#pragma endregion