#include <Framework/Object.h>

#pragma region "Constructors/Destructor"

Object::Object() : Node(), m_mesh(nullptr), m_material(nullptr)
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

#pragma region "Setters"

void Object::SetMesh(Mesh const * const & mesh)
{
	m_mesh = mesh;
}

#pragma endregion