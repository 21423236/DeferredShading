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

bool Object::IsRenderable() const
{
	return true;
}

#pragma endregion