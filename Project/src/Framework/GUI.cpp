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
		if (index == 0)
		{
			*outName = DefaultName;
			return true;
		}

		Scene * scene = (Scene*)data;
		*outName = scene->GetMaterialName(index-1);
		return true;
	}
};

struct MeshCombo {
	static bool choices(void * data, int index, char const ** outName)
	{
		if (index == 0)
		{
			*outName = DefaultName;
			return true;
		}

		Scene * scene = (Scene*)data;
		*outName = scene->GetMaterialName(index-1);
		return true;
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
			int currentMesh = 0;
			if (ImGui::Combo("", &currentMesh, MeshCombo::choices, &scene, scene.m_meshes.size()+1))
			{

			}
			ImGui::PopID();
			ImGui::PopItemWidth();
			ImGui::NextColumn();

			ImGui::Text("Material:");
			ImGui::NextColumn();
			
			ImGui::PushItemWidth(-1);
			ImGui::PushID(1);
			int currentMaterial = 0;
			if (ImGui::Combo("", &currentMaterial, MaterialCombo::choices, &scene, scene.m_materials.size()+1))
			{

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
	int currentDiffuse = 0;
	ImGui::PushID(100);
	if (ImGui::Combo("", &currentDiffuse, MaterialCombo::choices, &scene, scene.m_materials.size() + 1))
	{

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
	int currentNormal = 0;
	ImGui::PushID(101);
	if (ImGui::Combo("", &currentNormal, MaterialCombo::choices, &scene, scene.m_materials.size() + 1))
	{

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
	int currentSpecular = 0;
	ImGui::PushID(102);
	if (ImGui::Combo("", &currentSpecular, MaterialCombo::choices, &scene, scene.m_materials.size() + 1))
	{

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
		for (auto materialPair : scene.m_materials)
		{
			if (ImGui::TreeNode(materialPair.first.c_str()))
			{
				MaterialEditor(materialPair.second, scene);
				ImGui::TreePop();
			}
			ImGui::Separator();
		}
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{

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
				ImGui::EndTooltip();
			}
			ImGui::Text(texturePair.first.c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::Spacing();
		if (ImGui::Button("Load Texture..."))
		{
			ImGui::OpenPopup("Load Texture...");
		}

		if(ImGui::BeginPopupModal("Load Texture..."))
		{
			ImGui::Text("Path: ");
			ImGui::SameLine();
			static char pathBuffer[500];
			ImGui::InputText("", pathBuffer, 500);
			if (ImGui::Button("Load"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
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