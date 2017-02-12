#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Node
{

public:

	//constructors/destructor
	Node();
	Node(glm::vec3 const & translation, glm::quat const & orientation);
	~Node();

	//public methods
	void AddChild(Node * node);

	//getters
	glm::vec3 const & GetTranslation() const;
	glm::quat const & GetOrientation() const;

	//setters
	void SetTranslation(glm::vec3 const & translation);
	void SetOrientation(glm::quat const & orientation);

private:

	glm::vec3 m_translation;
	glm::quat m_orientation;
	std::vector<Node *> m_children;

};