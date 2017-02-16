#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class Node
{

public:

	friend class IRenderer;
	friend class Scene;
	friend class GUI;

	//constructors/destructor
	Node(std::string const & name);
	Node(std::string const & name, glm::vec3 const & translation, glm::quat const & orientation);
	virtual ~Node();

	//public methods
	void AddChild(Node * node);

	//getters
	glm::vec3 const & GetTranslation() const;
	glm::vec3 const & GetScale() const;
	glm::quat const & GetOrientation() const;
	glm::mat4 GetTransformMatrix() const;
	glm::mat4 GetTransformMatrixWithScale() const;

	//setters
	void SetTranslation(glm::vec3 const & translation);
	void SetScale(glm::vec3 const & scale);
	void SetOrientation(glm::quat const & orientation);

	typedef enum NodeType
	{
		BASE_NODE,
		OBJECT_NODE,
		LIGHT_NODE,
		MAX_NODE_TYPES
	} NodeType;

	virtual NodeType GetNodeType() const;

private:

	std::string m_name;
	glm::vec3 m_translation;
	glm::vec3 m_scale;
	glm::quat m_orientation;
	std::vector<Node *> m_children;

};