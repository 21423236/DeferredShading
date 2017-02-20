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

void GUI::TraverseGraph(Node * node)
{
	if (!node)
		return;

	for (auto child : node->m_children)
	{
		switch (child->GetNodeType())
		{
		case Node::OBJECT_NODE:
			if (ImGui::TreeNode(child, child->m_name.c_str()))
			{
				Object * object = dynamic_cast<Object*>(child);
				if (ImGui::TreeNode("Material"))
				{
					MaterialEditor(object->m_material);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}
				if (child->m_children.size())
					ImGui::Text("Children:");
				ImGui::Indent(16.0f);
				TraverseGraph(child);
				ImGui::Unindent(16.0f);
				ImGui::TreePop();
			}
			break;
		case Node::GLOBAL_LIGHT_NODE:
			if (ImGui::TreeNode(child, child->m_name.c_str()))
			{
				GlobalLight * light = dynamic_cast<GlobalLight*>(child);
				if (ImGui::TreeNode("Attributes"))
				{
					ImGui::InputFloat3("Is", &light->m_intensity[0]);
					ImGui::Image((void*)&light->m_shadowMap, ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}

				if (child->m_children.size())
					ImGui::Text("Children:");
				ImGui::Indent(16.0f);
				TraverseGraph(child);
				ImGui::Unindent(16.0f);
				
				ImGui::TreePop();
			}
			break;
		case Node::LOCAL_LIGHT_NODE:
			if (ImGui::TreeNode(child, child->m_name.c_str()))
			{
				LocalLight * light = dynamic_cast<LocalLight*>(child);
				if (ImGui::TreeNode("Attributes"))
				{
					ImGui::InputFloat3("Is", &light->m_intensity[0]);
					ImGui::InputFloat("R", &light->m_radius);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}

				if (child->m_children.size())
					ImGui::Text("Children");
				ImGui::Indent(16.0f);
				TraverseGraph(child);
				ImGui::Unindent(16.0f);

				ImGui::TreePop();
			}
			break;
		case Node::BASE_NODE:
			if (ImGui::TreeNode(child, child->m_name.c_str()))
			{
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}
				
				if (child->m_children.size())
					ImGui::Text("Children:");
				ImGui::Indent(16.0f);
				TraverseGraph(child);
				ImGui::Unindent(16.0f);

				ImGui::TreePop();
			}
			break;
		}
	}
}

void GUI::MaterialEditor(Material * material)
{
	ImGui::InputFloat3("Kd", &material->GetKd()[0]);
	ImGui::InputFloat3("Ks", &material->GetKs()[0]);
	ImGui::InputFloat("A", &material->GetAlpha());
}

void GUI::NodeInformation(Node * node)
{
	ImGui::InputFloat3("T", &node->m_translation[0]);
	ImGui::InputFloat4("R", &node->m_orientation[0]);
	ImGui::InputFloat3("S", &node->m_scale[0]);
}

#define MAX(a, b)(a>b?a:b)
#define MIN(a, b)(a<b?a:b)

void GUI::GenerateGUI(Scene & scene)
{
	ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Scene", 0, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Scene Graph"))
	{
		static bool checkBox;
		ImGui::Checkbox("Checkbox", &checkBox);
		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Materials"))
	{

		ImGui::Spacing();
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{

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
			ImGui::Image((void*)texturePair.second, ImVec2(100, 100));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("This is a tooltip");
				ImGui::EndTooltip();
			}
			ImGui::Text(texturePair.first.c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
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