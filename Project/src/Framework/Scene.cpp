#include <Framework/Application.h>
#include <Framework/Scene.h>
#include <Framework/Mesh.h>
#include <Framework/Material.h>
#include <Framework/IRenderPass.h>
#include <Framework/Defaults.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <png/png.h>
#include <GL/glew.h>

#pragma region "Constructors/Destructor"

Scene::Scene(Application & application, unsigned const & windowWidth, unsigned const & windowHeight) : m_application(application), m_rootNode(new Node("Root")), m_camera(m_viewMatrix), m_meshes(), m_materials(), m_textures(), m_projectionMatrix(), m_viewMatrix(), m_sceneSize(glm::vec3(1, 1, 1)), m_ambientIntensity(glm::vec3(0, 0, 0)), m_windowWidth(windowWidth), m_windowHeight(windowHeight)
{

}

Scene::~Scene()
{
	delete m_rootNode;
}

#pragma endregion

#pragma region "Getters"

Camera const & Scene::GetCamera() const
{
	return m_camera;
}

Camera & Scene::GetCamera()
{
	return m_camera;
} 

glm::mat4 const & Scene::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

glm::mat4 const & Scene::GetViewMatrix() const
{
	return m_viewMatrix;
}

glm::vec3 const & Scene::GetSceneSize() const
{
	return m_sceneSize;
}

glm::vec3 const & Scene::GetAmbientIntensity() const
{
	return m_ambientIntensity;
}

#pragma endregion

#pragma region "Setters"

void Scene::SetProjection(float const & ry, float const & front, float const & back)
{
	float rx = ry * (float)m_windowWidth / (float)m_windowHeight;
	m_projectionMatrix = glm::mat4(
		glm::vec4(1.0f / rx, 0.0f, 0.0f, 0.0f), 
		glm::vec4(0.0f, 1.0f / ry, 0.0f, 0.0f), 
		glm::vec4(0.0f, 0.0f, (front - back) / (back - front), -1.0f), 
		glm::vec4(0.0f, 0.0f, (-2.0f * front * back) / (back - front), 0.0f));
}

void Scene::SetWindowWidth(unsigned const & width)
{
	m_windowWidth = width;
}

void Scene::SetWindowHeight(unsigned const & height)
{
	m_windowHeight = height;
}

void Scene::SetSceneSize(glm::vec3 const & sceneSize)
{
	m_sceneSize = sceneSize;
}

void Scene::SetAmbientIntensity(glm::vec3 const & ambientIntensity)
{
	m_ambientIntensity = ambientIntensity;
}

#pragma endregion

#pragma region "Public Methods"

Mesh * Scene::CreateMesh(std::string const & name, std::string const & path)
{
	Assimp::Importer importer;
	aiScene const * scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate);

	if (!scene || !scene->HasMeshes())
		return nullptr;

	//process the object
	aiMesh * assimpMesh = scene->mMeshes[0];
	Mesh * mesh = new Mesh(assimpMesh->mNumFaces, assimpMesh->mFaces, assimpMesh->mVertices, assimpMesh->mNormals, assimpMesh->mTangents, assimpMesh->mTextureCoords);
	MeshInfo meshInfo = { path, mesh, 0 };
	m_meshes.push_back(std::make_pair(name, meshInfo));
	return mesh;
}

Material * Scene::CreateMaterial(std::string const & name, glm::vec3 const & kd, glm::vec3 const & ks, float const & alpha)
{
	Material * material = new Material(kd, ks, alpha);
	MaterialInfo materialInfo = { material, 0 };
	m_materials.push_back(std::make_pair(name, materialInfo));
	return material;
}

Texture * Scene::CreateTexture(std::string const & name, std::string const & path, bool gamma)
{
	Texture * texture = new Texture();

	png_byte header[8];

	FILE *fp;
	fopen_s(&fp, path.c_str(), "rb");

	if (fp == 0)
	{
		return nullptr;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		fclose(fp);
		return nullptr;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		fclose(fp);
		return nullptr;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return nullptr;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return nullptr;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return nullptr;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	if (color_type == PNG_COLOR_TYPE_RGB)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if (gamma)
	{
		png_set_gamma(png_ptr, 2.2, 0.45455);
	}

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes - 1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte * image_data;
	image_data = (png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte) + 15);
	if (image_data == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return nullptr;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep * row_pointers = (png_bytep*)malloc(temp_height * sizeof(png_bytep));
	if (row_pointers == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return nullptr;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	int i;
	for (i = 0; i < temp_height; i++)
	{
		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	

	// initialize texture object
	texture->Initialize(temp_width, temp_height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(fp);

	struct TextureInfo info = { path, texture, 0 };
	m_textures.push_back(std::make_pair(name, info));
	return texture;
}

std::string Scene::OpenFile(char const * filter)
{
	return m_application.OpenFile(filter);
}

void Scene::AddNode(Node * node)
{
	m_rootNode->AddChild(node);
}

void Scene::Traverse(IRenderPass const & pass) const
{
	static glm::mat4 identity = glm::mat4();
	for (auto & const node : m_rootNode->m_children)
	{
		TraverseNode(node, pass, identity);
	}
}

void Scene::Resize(int const & width, int const & height)
{
	float ry = 1.0f / m_projectionMatrix[1][1];
	float rx = ry * ((float)width / (float)height);
	m_projectionMatrix[0][0] = 1.0f / rx;
	m_windowWidth = width;
	m_windowHeight = height;
}

void Scene::FreeMemory()
{
	for (auto mesh : m_meshes)
		delete mesh.second.mesh;
	m_meshes.clear();

	for (auto material : m_materials)
		delete material.second.material;
	m_meshes.clear();

	for (auto texture : m_textures)
		delete texture.second.texture;
	m_textures.clear();
}

#pragma endregion

#pragma region "Reference Counting"

void Scene::IncrementReference(Material * material)
{
	for (auto & materialPair : m_materials)
		if (materialPair.second.material == material)
		{
			materialPair.second.referenceCount++;
			break;
		}
}

void Scene::DecrementReference(Material * material)
{
	for (auto & materialPair : m_materials)
		if (materialPair.second.material == material)
		{
			materialPair.second.referenceCount--;
			break;
		}
}

void Scene::IncrementReference(Mesh * mesh)
{
	for (auto & meshPair : m_meshes)
		if (meshPair.second.mesh == mesh)
		{
			meshPair.second.referenceCount++;
			break;
		}
}

void Scene::DecrementReference(Mesh * mesh)
{
	for (auto & meshPair : m_meshes)
		if (meshPair.second.mesh == mesh)
		{
			meshPair.second.referenceCount--;
			break;
		}
}

void Scene::IncrementReference(Texture const * texture)
{
	if (!texture)
		return;

	for (auto & texturePair : m_textures)
		if (texturePair.second.texture == texture)
		{
			texturePair.second.referenceCount++;
			break;
		}
}

void Scene::DecrementReference(Texture const * texture)
{
	if (!texture)
		return;

	for (auto & texturePair : m_textures)
		if (texturePair.second.texture == texture)
		{
			texturePair.second.referenceCount--;
			break;
		}
}


#pragma endregion

#pragma region "Private Methods"

void Scene::TraverseNode(Node * const & node, IRenderPass const & pass, glm::mat4 const & modelMatrix) const
{
	pass.ProcessNode(node, modelMatrix * node->GetTransformMatrixWithScale());
	for (auto & const child : node->m_children)
		TraverseNode(child, pass, modelMatrix * node->GetTransformMatrix());
}

#pragma endregion