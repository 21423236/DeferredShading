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
					ImGui::Image((void*)(intptr_t)light->m_shadowMap.m_handle, ImVec2(150, 150), ImVec2(0, 1), ImVec2(1, 0));
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