#include <Framework/Node.h>


#pragma region "Constructors/Destructor"
 
Node::Node() : m_translation(0.0f, 0.0f, 0.0f), m_orientation(), m_children()
{

}

Node::Node(glm::vec3 const & translation, glm::quat const & orientation) : m_translation(translation), m_orientation(orientation), m_children()
{

}


Node::~Node()
{

}

#pragma endregion

#pragma region "Public Methods"

void Node::AddChild(Node * node)
{
	m_children.push_back(node);
}

#pragma endregion

#pragma region "Getters"

glm::vec3 const & Node::GetTranslation() const
{
	return m_translation;
}

glm::quat const & Node::GetOrientation() const
{
	return m_orientation;
}

#pragma endregion

#pragma region "Setters"

void Node::SetTranslation(glm::vec3 const & translation)
{
	m_translation = translation;
}

void Node::SetOrientation(glm::quat const & orientation)
{
	m_orientation = orientation;
}

#pragma endregion