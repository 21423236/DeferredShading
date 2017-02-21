#pragma once

#include "Camera.h"
#include "Node.h"
#include <glm/glm.hpp>
#include <map>
#include <map>
#include <string>


class Mesh;
class Material;
class IRenderPass;
class Texture;

class Scene
{

public:

	struct MeshInfo
	{
		std::string path;
		Mesh * mesh;
		int referenceCount;
	};

	struct MaterialInfo
	{
		Material * material;
		int referenceCount;
	};

	struct TextureInfo
	{
		std::string path;
		Texture * texture;
		int referenceCount;
	};

	friend class IRenderer;
	friend class GUI;

	//constructors/destructor
	Scene(unsigned const & windowWidth, unsigned const & windowHeight);
	~Scene();

	//getters
	Camera const & GetCamera() const;
	Camera & GetCamera();
	glm::mat4 const & GetProjectionMatrix() const;
	glm::mat4 const & GetViewMatrix() const;
	glm::vec3 const & GetSceneSize() const;
	glm::vec3 const & GetAmbientIntensity() const;

	//setters
	void SetProjection(float const & ry, float const & front, float const & back);
	void SetSceneSize(glm::vec3 const & sceneSize);
	void SetAmbientIntensity(glm::vec3 const & ambientIntensity);
	void SetWindowWidth(unsigned const & width);
	void SetWindowHeight(unsigned const & height);

	//public methods
	Mesh * CreateMesh(std::string const & name, std::string const & path);
	Material * CreateMaterial(std::string const & name, glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha);
	Texture * CreateTexture(std::string const & name, std::string const & path, bool gamma = true);

	void AddNode(Node * node);
	void Traverse(IRenderPass const & pass) const;
	void Resize(int const & width, int const & height);
	void FreeMemory();

	//reference counting
	void IncrementReference(Material * material);
	void DecrementReference(Material * material);
	void IncrementReference(Mesh * mesh);
	void DecrementReference(Mesh * mesh);
	void IncrementReference(Texture const * texture);
	void DecrementReference(Texture const * texture);

private:

	//private methods
	void TraverseNode(Node * const & node, IRenderPass const & pass, glm::mat4 const & modelMatrix) const;

	Node * m_rootNode;

	Camera m_camera;

	std::vector<std::pair<std::string, struct MeshInfo>> m_meshes;
	std::vector < std::pair<std::string, struct MaterialInfo>> m_materials;

	std::vector<std::pair<std::string, struct TextureInfo>> m_textures;

	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::vec3 m_sceneSize;
	glm::vec3 m_ambientIntensity;

	unsigned m_windowWidth;
	unsigned m_windowHeight;

};

