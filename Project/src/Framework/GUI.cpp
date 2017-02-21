#include <Framework/GUI.h>
#include <Framework/Window.h>
#include <Framework/Scene.h>
#include <Framework/Material.h>
#include <Framework/Node.h>
#include <Framework/Object.h>
#include <Framework/Mesh.h>
#include <Framework/GlobalLight.h>
#include <Framework/LocalLight.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl.h>

#define _USE_MATH_DEFINES
#include <math.h>

struct Grapher {
	static float graph(void * data, int idx)
	{
		float const & alpha = ((Material*)data)->GetAlpha();
		float radian = (float)idx*(M_PI / 70.0f);
		float result = ((alpha + 2.0f) / 2 * M_PI) * powf(sinf(radian), alpha);
		return result;
	}
};

static char const * DefaultName = "None";

struct MaterialCombo {
	static bool choices (void * data, int index, char const ** outName)
	{
		std::vector<std::pair<std::string, struct Scene::MaterialInfo>> * materials = (std::vector<std::pair<std::string, struct Scene::MaterialInfo>>*)data;
		*outName = (*materials)[index].first.c_str();
		return true;
	}

	static int indexForMaterial(Material * material, std::vector<std::pair<std::string, struct Scene::MaterialInfo>>* materials)
	{
		for (int i = 0; i < materials->size(); ++i)
		{
			if ((*materials)[i].second.material == material)
				return i;
		}

		return 0;
	}
};

struct MeshCombo {
	static bool choices(void * data, int index, char const ** outName)
	{
		std::vector<std::pair<std::string, struct Scene::MeshInfo>> *meshes = (std::vector<std::pair<std::string, struct Scene::MeshInfo>>*)data;
		*outName = (*meshes)[index].first.c_str();
		return true;
	}

	static int indexForMesh(Mesh * mesh, std::vector<std::pair<std::string, struct Scene::MeshInfo>>* meshes)
	{
		for (int i = 0; i < meshes->size(); ++i)
		{
			if ((*meshes)[i].second.mesh == mesh)
				return i;
		}

		return 0;
	}
};

struct TextureCombo {
	static bool choices(void * data, int index, char const ** outName)
	{
		if (index == 0)
		{
			*outName = DefaultName;
			return true;
		}

		std::vector<std::pair<std::string, struct Scene::TextureInfo>>* textures = (std::vector<std::pair<std::string, struct Scene::TextureInfo>>*)data;
		*outName = (*textures)[index-1].first.c_str();
		return true;
	}

	static int indexForTexture(Texture const * texture, std::vector<std::pair<std::string, struct Scene::TextureInfo>>* textures)
	{
		if (!texture)
			return 0;

		for (int i = 1; i <= textures->size(); ++i)
		{
			if ((*textures)[i - 1].second.texture == texture)
				return i;
		}

		return 0;
	}
};


GUI::GUI()
{
}


GUI::~GUI()
{
}

void GUI::Initialize(Window const & window)
{
	ImGui_Impl_Init(window.GetHandle());
}

void GUI::NewFrame(int const & width, int const & height)
{
	ImGui_Impl_NewFrame(width, height);
}

#define MAX(a, b)(a>b?a:b)
#define MIN(a, b)(a<b?a:b)

void GUI::TraverseNode(Node * node, Scene & scene)
{
	if (!node)
		return;
	ImGui::Separator();
	ImGui::PushID(node);
	ImGui::AlignFirstTextHeightToWidgets();
	
	bool node_open;

	if (node->GetNodeType() == Node::BASE_NODE)
		node_open = ImGui::TreeNode("Node");
	else if (node->GetNodeType() == Node::OBJECT_NODE)
		node_open = ImGui::TreeNode("Object");
	else if (node->GetNodeType() == Node::GLOBAL_LIGHT_NODE)
		node_open = ImGui::TreeNode("Global Light");
	else if (node->GetNodeType() == Node::LOCAL_LIGHT_NODE)
		node_open = ImGui::TreeNode("Local Light");

	ImGui::NextColumn();
	ImGui::AlignFirstTextHeightToWidgets();
	ImGui::Text(node->GetName().c_str());

	ImGui::NextColumn();

	if (node_open)
	{
		if (node->GetNodeType() == Node::OBJECT_NODE)
		{
			Object * object = dynamic_cast<Object*>(node);

			ImGui::Text("Mesh:");
			ImGui::NextColumn();
			
			ImGui::PushItemWidth(-1);
			ImGui::PushID(0);
			int currentMesh = MeshCombo::indexForMesh(object->m_mesh, &scene.m_meshes);
			if (ImGui::Combo("", &currentMesh, MeshCombo::choices, &scene.m_meshes, scene.m_meshes.size()))
			{
				scene.DecrementReference(object->m_mesh);
				object->m_mesh = scene.m_meshes[currentMesh].second.mesh;
				scene.m_meshes[currentMesh].second.referenceCount++;
			}
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Material:");
			ImGui::NextColumn();
			
			ImGui::PushItemWidth(-1);
			ImGui::PushID(1);
			int currentMaterial = MaterialCombo::indexForMaterial(object->m_material, &scene.m_materials);
			if (ImGui::Combo("", &currentMaterial, MaterialCombo::choices, &scene.m_materials, scene.m_materials.size()))
			{
				scene.DecrementReference(object->m_material);
				object->m_material = scene.m_materials[currentMaterial].second.material;
				scene.m_materials[currentMaterial].second.referenceCount++;
			}
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();
		}
		else if (node->GetNodeType() == Node::GLOBAL_LIGHT_NODE)
		{
			GlobalLight * globalLight = dynamic_cast<GlobalLight*>(node);

			ImGui::Text("Intensity:");
			ImGui::NextColumn();
			
			ImGui::PushItemWidth(-1);
			ImGui::PushID(0);
			ImGui::InputFloat3("", &globalLight->m_intensity[0]);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Shadow Map:");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::Image((void*)&globalLight->m_shadowMap, ImVec2(100, 100));
			ImGui::PopItemWidth();
			ImGui::NextColumn();
		}
		else if (node->GetNodeType() == Node::LOCAL_LIGHT_NODE)
		{
			LocalLight * localLight = dynamic_cast<LocalLight*>(node);

			ImGui::Text("Intensity:");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::PushID(0);
			ImGui::InputFloat3("", &localLight->m_intensity[0]);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Radius:");
			ImGui::NextColumn();

			ImGui::PushItemWidth(-1);
			ImGui::PushID(1);
			ImGui::InputFloat("", &localLight->m_radius);
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();
		}

		ImGui::Text("Translation:");
		ImGui::NextColumn();
		
		ImGui::PushItemWidth(-1);
		ImGui::PushID(10);
		ImGui::InputFloat3("", &node->m_translation[0]);
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();
		
		ImGui::Text("Scale:");
		ImGui::NextColumn();
		
		ImGui::PushItemWidth(-1);
		ImGui::PushID(11);
		ImGui::InputFloat3("", &node->m_scale[0]);
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Text("Orientation");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::PushID(12);
		ImGui::InputFloat4("", &node->m_orientation[0]);
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		for (auto child : node->m_children)
			TraverseNode(child, scene);
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void GUI::MaterialEditor(Material * material, Scene & scene)
{
	ImGui::PushItemWidth(-1);
	ImGui::Text("Kd:   ");ImGui::SameLine();
	ImGui::DragFloat3("Kd", &material->m_kd[0], 0.01f, 0.0f, 1.0f);
	ImGui::Text("Ks:   ");ImGui::SameLine();
	ImGui::DragFloat3("Ks", &material->m_ks[0], 0.01f, 0.0f, 1.0f);
	ImGui::Text("Alpha:"); ImGui::SameLine();
	ImGui::SliderFloat("Alpha", &material->m_alpha, 1.0f, 500.0f);
	ImGui::Text("D(H): "); ImGui::SameLine();
	ImGui::PlotLines("D(H)", Grapher::graph, material, 70, 0, NULL, 0.0f, 500.0f, ImVec2(0, 60));
	ImGui::PopItemWidth();
	ImGui::Spacing();

	ImGui::Columns(3);
	ImGui::Text("D:");
	ImGui::SameLine();
	ImGui::PushID(100);
	int currentDiffuse = TextureCombo::indexForTexture(material->m_diffuseMap, &scene.m_textures);
	if (ImGui::Combo("", &currentDiffuse, TextureCombo::choices, &scene.m_textures, scene.m_textures.size() + 1))
	{
		scene.DecrementReference(material->m_diffuseMap);
		if (currentDiffuse == 0)
			material->m_diffuseMap = nullptr;
		else
		{
			material->m_diffuseMap = scene.m_textures[currentDiffuse - 1].second.texture;
			scene.m_textures[currentDiffuse - 1].second.referenceCount++;
		}
	}
	ImGui::PopID();
	if (material->HasDiffuseMap())
	{
		ImGui::Spacing();
		ImGui::Image((void*)(material->m_diffuseMap), ImVec2(100, 100));
	}

	ImGui::NextColumn();
	ImGui::Text("N:");
	ImGui::SameLine();
	ImGui::PushID(101);
	int currentNormal = TextureCombo::indexForTexture(material->m_normalMap, &scene.m_textures);
	if (ImGui::Combo("", &currentNormal, TextureCombo::choices, &scene.m_textures, scene.m_textures.size() + 1))
	{
		scene.DecrementReference(material->m_normalMap);
		if (currentNormal == 0)
			material->m_normalMap = nullptr;
		else
		{
			material->m_normalMap = scene.m_textures[currentNormal - 1].second.texture;
			scene.m_textures[currentNormal - 1].second.referenceCount++;
		}
	}
	ImGui::PopID();
	if (material->HasNormalMap())
	{
		ImGui::Spacing();
		ImGui::Image((void*)(material->m_normalMap), ImVec2(100, 100));
	}

	ImGui::NextColumn();
	ImGui::Text("S:");
	ImGui::SameLine();
	ImGui::PushID(102);
	int currentSpecular = TextureCombo::indexForTexture(material->m_specularMap, &scene.m_textures);
	if (ImGui::Combo("", &currentSpecular, TextureCombo::choices, &scene.m_textures, scene.m_textures.size() + 1))
	{
		scene.DecrementReference(material->m_specularMap);
		if (currentSpecular == 0)
			material->m_specularMap = nullptr;
		else
		{
			material->m_specularMap = scene.m_textures[currentSpecular - 1].second.texture;
			scene.m_textures[currentSpecular - 1].second.referenceCount++;
		}
	}
	ImGui::PopID();
	if (material->HasSpecularMap())
	{
		ImGui::Spacing();
		ImGui::Image((void*)(material->m_specularMap), ImVec2(100, 100));
	}

	ImGui::Columns(1);
	ImGui::Spacing();
}

void GUI::GenerateGUI(Scene & scene)
{
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Scene", 0, ImGuiWindowFlags_ShowBorders))
	{
		ImGui::End();
		return;
	}

	ImGui::Columns(2);

	ImGui::Text("Scene Size:");
	ImGui::NextColumn();

	ImGui::PushItemWidth(-1);
	ImGui::PushID(300);
	ImGui::InputFloat3("", &scene.m_sceneSize[0]);
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	ImGui::Text("Ambient Intensity:");
	ImGui::NextColumn();

	ImGui::PushItemWidth(-1);
	ImGui::PushID(301);
	ImGui::InputFloat3("", &scene.m_ambientIntensity[0]);
	ImGui::PopID();
	ImGui::PopItemWidth();
	ImGui::NextColumn();

	ImGui::Columns(1);
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Columns(2);
		ImGui::Text("Position:");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::PushID(200);
		if (ImGui::InputFloat2("", &scene.m_camera.m_position[0]))
			scene.m_camera.UpdateViewMatrix();
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Text("Zoom:");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::PushID(203);
		if (ImGui::InputFloat("", &scene.m_camera.m_zoom))
			scene.m_camera.UpdateViewMatrix();
		ImGui::PopID();
		ImGui::PopItemWidth;
		ImGui::NextColumn();

		ImGui::Text("Spin:");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::PushID(201);
		if (ImGui::DragFloat("", &scene.m_camera.m_spin, 0.01f, 0.0f, 2 * M_PI))
			scene.m_camera.UpdateViewMatrix();
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::NextColumn();

		ImGui::Text("Tilt:");
		ImGui::NextColumn();

		ImGui::PushItemWidth(-1);
		ImGui::PushID(202);
		if (ImGui::DragFloat("", &scene.m_camera.m_tilt, 0.01f, 0.0f, 2 * M_PI))
			scene.m_camera.UpdateViewMatrix();
		ImGui::PopID();
		ImGui::PopItemWidth();

		ImGui::Columns(1);

	}

	if (ImGui::CollapsingHeader("Scene Graph"))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
		ImGui::Columns(2);
		for (auto child : scene.m_rootNode->m_children)
			TraverseNode(child, scene);
		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::Separator();
	}

	if (ImGui::CollapsingHeader("Materials"))
	{
		for (auto & materialPair : scene.m_materials)
		{
			if (ImGui::TreeNode(materialPair.first.c_str(), "%s (%i)", materialPair.first.c_str(), materialPair.second.referenceCount))
			{
				MaterialEditor(materialPair.second.material, scene);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{
		ImGui::Columns(4);
		ImGui::Text("Name");
		ImGui::NextColumn();
		ImGui::Text("Path");
		ImGui::NextColumn();
		ImGui::Text("Referenced");
		ImGui::NextColumn();
		ImGui::Text("Actions");
		ImGui::NextColumn();
		ImGui::Separator();
		for (auto & meshPair : scene.m_meshes)
		{
			ImGui::Text(meshPair.first.c_str());
			ImGui::NextColumn();
			ImGui::Text(meshPair.second.path.c_str());
			ImGui::NextColumn();
			ImGui::Text("%i", meshPair.second.referenceCount);
			ImGui::NextColumn();
			if (ImGui::Button("Delete"))
			{

			}
			ImGui::NextColumn();
			ImGui::Separator();
		}
		ImGui::Columns(1);
		ImGui::Spacing();
		if (ImGui::Button("Load Mesh"))
		{
			std::string path = scene.OpenFile("Wavefront OBJ\0*.obj\0");
			if (path != "")
				scene.CreateMesh(std::string(path.end() - 5, path.end()), path);
		}
		ImGui::Spacing();
	}

	if(ImGui::CollapsingHeader("Textures"))
	{
		static float const textureDisplaySize = 100.0f;
		float width = ImGui::GetWindowWidth();
		int numberOfColumns = MIN(MAX((int)(width / textureDisplaySize), 1), scene.m_textures.size());
		ImGui::Columns(numberOfColumns);
		for (auto texturePair : scene.m_textures)
		{
			ImGui::Image((void*)texturePair.second.texture, ImVec2(100, 100));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("Size: %i x %i", texturePair.second.texture->GetWidth(), texturePair.second.texture->GetHeight());
				ImGui::Text("Path: %s", texturePair.second.path.c_str());
				ImGui::Text("Referenced: %i", texturePair.second.referenceCount);
				ImGui::EndTooltip();
			}
			ImGui::Text(texturePair.first.c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Spacing();
		
		if (ImGui::Button("Load Texture"))
		{
			std::string path = scene.OpenFile("PNG\0*.png\0");
			if(path != "")
				scene.CreateTexture(std::string(path.end() - 5, path.end()), path, true);
		}

		ImGui::Separator();
	}
	
	ImGui::End();
}

void GUI::EndFrame()
{
	ImGui::Render();
}

void GUI::Finalize()
{
	ImGui_Impl_Shutdown();
}