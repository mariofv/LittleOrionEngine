#include "GameObject.h"
#include "Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "Texture.h"

#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include "IconsFontAwesome5.h"

GameObject::GameObject()
{
	this->transform = (ComponentTransform*)CreateComponent(Component::ComponentType::TRANSFORM);
}

GameObject::GameObject(const std::string name) :
	name(name)
{
	this->transform = (ComponentTransform*)CreateComponent(Component::ComponentType::TRANSFORM);
}


GameObject::~GameObject()
{
	delete transform;
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		delete components[i];
	}
	components.clear();

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		delete children[i];
	}
	children.clear();
}

void GameObject::Update()
{
	GLuint shader_program = App->program->texture_program;
	glUseProgram(shader_program);

	transform->Render(shader_program);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "view"),
		1,
		GL_TRUE,
		&App->cameras->view[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "proj"),
		1,
		GL_TRUE,
		&App->cameras->proj[0][0]
	);

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == Component::ComponentType::MESH)
		{
			ComponentMesh* current_mesh = (ComponentMesh*)components[i];
			int mesh_material_index = current_mesh->material_index;
			const GLuint mesh_texture = GetMaterialTexture(mesh_material_index);
			current_mesh->Render(shader_program, mesh_texture);
		}
	}

	glUseProgram(0);

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Update();
	}

}

void GameObject::AddChild(GameObject *child)
{
	child->parent = this;
	children.push_back(child);
}

Component* GameObject::CreateComponent(const Component::ComponentType type)
{
	Component *created_component;
	switch (type)
	{
	case Component::ComponentType::MATERIAL:
		created_component = App->texture->CreateComponentMaterial();
		break;

	case Component::ComponentType::MESH:
		created_component = App->renderer->CreateComponentMesh();
		break;

	case Component::ComponentType::TRANSFORM:
		created_component = new ComponentTransform();
		break;

	default:
		APP_LOG_ERROR("Error creating component. Incorrect component type.");
		return nullptr;
	}

	created_component->owner = this;
	components.push_back(created_component);

	return created_component;
}

const GLuint GameObject::GetMaterialTexture(const int material_index)
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == Component::ComponentType::MATERIAL)
		{
			ComponentMaterial* current_material = (ComponentMaterial*)components[i];
			if (current_material->index == material_index)
			{
				return current_material->texture->opengl_texture;
			}
		}
	}

	return 0;
}

void GameObject::ShowPropertiesWindow()
{
	ImGui::Checkbox("", &active);

	ImGui::SameLine();
	ImGui::Text(ICON_FA_CUBE);

	ImGui::SameLine();
	ImGui::InputText("###GameObject name Input", &name);

	ImGui::Spacing();
	ImGui::Separator();

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (i != 0)
		{
			ImGui::Spacing();
			ImGui::Separator();
		}
		ImGui::Spacing();
		ImGui::PushID(i);
		components[i]->ShowComponentWindow();
		ImGui::PopID();
	}
}

void GameObject::ShowGameObjectHierarchy()
{
	std::string game_object_name_label = (std::string(ICON_FA_CUBE) + " " + name);
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	if (children.size() == 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}
	if (App->scene->selected_game_object == this)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (ImGui::TreeNodeEx(game_object_name_label.c_str(), flags))
	{
		if (ImGui::IsItemClicked())
		{
			App->scene->selected_game_object = this;
		}
		ShowGameObjectActionsMenu(game_object_name_label); // THIS IS NEEDED WHEN TREE IS EXPANDED
		for (int i = 0; i < children.size(); i++)
		{
			ImGui::PushID(i);
			children[i]->ShowGameObjectHierarchy();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ShowGameObjectActionsMenu(game_object_name_label); // THIS IS NEEDED WHEN TREE NODE IS COLLAPSED
}

void GameObject::ShowGameObjectActionsMenu(const std::string label)
{
	if (ImGui::BeginPopupContextItem(label.c_str()))
	{
		if (ImGui::Selectable("Create GameObject"))
		{
			AddChild(new GameObject(App->scene->GetNextGameObjectName()));
		}
		ImGui::EndPopup();
	}
}