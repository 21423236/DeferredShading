#include <Framework/GUI.h>
#include <Framework/Window.h>
#include <Framework/Scene.h>
#include <Framework/Material.h>
#include <Framework/Node.h>
#include <Framework/Light.h>
#include <Framework/Object.h>
#include <Framework/Mesh.h>
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
			if (ImGui::TreeNode(child, "Object"))
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
				TraverseGraph(child);
				ImGui::TreePop();
			}
			break;
		case Node::LIGHT_NODE:
			if (ImGui::TreeNode(child, "Light"))
			{
				Light * light = dynamic_cast<Light*>(child);
				if (ImGui::TreeNode("Attributes"))
				{
					ImGui::ColorEdit3("Ia", &light->m_ambient[0]);
					ImGui::ColorEdit3("Is", &light->m_intensity[0]);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}
				TraverseGraph(child);
				ImGui::TreePop();
			}
			break;
		case Node::BASE_NODE:
			if (ImGui::TreeNode(child, "Node"))
			{
				if (ImGui::TreeNode("Transform"))
				{
					NodeInformation(child);
					ImGui::TreePop();
				}
				TraverseGraph(child);
				ImGui::TreePop();
			}
			break;
		}
	}
}

void GUI::MaterialEditor(Material * material)
{
	ImGui::ColorEdit3("Kd", &material->GetKd()[0]);
	ImGui::ColorEdit3("Ks", &material->GetKs()[0]);
	ImGui::DragFloat("A", &material->GetAlpha(), 1.0f, 0.0f, 120.0f);
}

void GUI::NodeInformation(Node * node)
{
	ImGui::InputFloat3("T", &node->m_translation[0]);
	ImGui::InputFloat4("R", &node->m_orientation[0]);
	ImGui::InputFloat3("S", &node->m_scale[0]);
}

void GUI::GenerateGUI(Scene & scene)
{
	ImGui::Begin("Scene");

	if (ImGui::TreeNode("Scene Graph"))
	{
		if (ImGui::TreeNode("Root"))
		{
			TraverseGraph(scene.m_rootNode);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	ImGui::Separator();
	
	if (ImGui::TreeNode("Materials"))
	{
		for (auto materialPair : scene.m_materials)
		{
			if (ImGui::TreeNode(materialPair.first.c_str()))
			{
				MaterialEditor(materialPair.second);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
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