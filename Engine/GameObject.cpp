#include "GameObject.h"
#include "Application.h"
#include "Hierarchy.h"
#include "BoundingBoxRenderer.h"
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
#include <FontAwesome5/IconsFontAwesome5.h>

#include <algorithm>

GameObject::GameObject() : transform(this), aabb_collider(this)
{
}

GameObject::GameObject(const std::string name) :
	name(name),
	transform(this),
	aabb_collider(this)
{

}


GameObject::~GameObject()
{
	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

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
	transform.GenerateGlobalModelMatrix();
	aabb_collider.GenerateBoundingBox();
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Update();
	}

}

void GameObject::Render() const
{
	if (!active)
	{
		return;
	}

	GLuint shader_program = App->program->texture_program;
	glUseProgram(shader_program);

	transform.Render(shader_program);
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

	if (parent != nullptr) // IS NOT ROOT NODE
	{
		aabb_collider.Render(App->program->default_program);
	}

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Render();
	}
}

void GameObject::SetParent(GameObject *new_parent)
{
	if (new_parent == parent)
	{
		return;
	}

	if (parent != nullptr) 
	{
		parent->RemoveChild(this);
	}
	new_parent->AddChild(this);
}


GameObject* GameObject::CreateChild(std::string name)
{
	if (name == "")
	{
		name = App->scene->hierarchy.GetNextGameObjectName();
	}

	GameObject *created_child = new GameObject(name);
	AddChild(created_child);

	return created_child;
}

void GameObject::AddChild(GameObject *child)
{
	if (child->parent != nullptr)
	{
		child->parent->RemoveChild(child);
	}
	
	child->parent = this;
	child->UpdateHierarchyDepth();
	child->UpdateHierarchyBranch();

	child->transform.ChangeLocalSpace(transform.GetGlobalModelMatrix());
	children.push_back(child);
}

void GameObject::RemoveChild(GameObject *child)
{
	std::vector<GameObject*>::iterator found = std::find(children.begin(), children.end(), child);
	if (found == children.end())
	{
		APP_LOG_ERROR("Incosistent GameObject Tree.");
	}
	children.erase(found);
	child->parent = nullptr;
	child->hierarchy_depth = 0;
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
	default:
		APP_LOG_ERROR("Error creating component. Incorrect component type.");
		return nullptr;
	}

	created_component->owner = this;
	components.push_back(created_component);

	return created_component;
}

Component*  GameObject::GetComponent(const Component::ComponentType type) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == Component::ComponentType::MESH)
		{
			return components[i];
		}
	}
	return nullptr;
}

void GameObject::MoveUpInHierarchy()
{
	std::vector<GameObject*>::iterator silbings_position = std::find(parent->children.begin(), parent->children.end(), this);
	if (silbings_position == parent->children.end())
	{
		APP_LOG_ERROR("Incosistent GameObject Tree.");
	}

	std::vector<GameObject*>::iterator swapped_silbing = max(silbings_position - 1, parent->children.begin());
	std::iter_swap(silbings_position, swapped_silbing);
}

void GameObject::MoveDownInHierarchy()
{
	std::vector<GameObject*>::iterator silbings_position = std::find(parent->children.begin(), parent->children.end(), this);
	if (silbings_position == parent->children.end())
	{
		APP_LOG_ERROR("Incosistent GameObject Tree.");
	}

	std::vector<GameObject*>::iterator swapped_silbing = min(silbings_position + 1, parent->children.end() - 1);
	std::iter_swap(silbings_position, swapped_silbing);
}

bool GameObject::IsAboveInHierarchy(const GameObject &potential_child) const
{
	return (
		this->hierarchy_branch == potential_child.hierarchy_branch
		&& this->hierarchy_depth < potential_child.hierarchy_depth
	);
}

void GameObject::UpdateHierarchyDepth()
{
	hierarchy_depth = parent->hierarchy_depth + 1;
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->UpdateHierarchyDepth();
	}
}

void GameObject::UpdateHierarchyBranch()
{
	if (parent->hierarchy_branch == 0) // PARENT IS ROOT GAMEOBJECT
	{
		hierarchy_branch = App->scene->hierarchy.GetNextBranch();
	}
	else
	{
		hierarchy_branch = parent->hierarchy_branch;
	}

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->UpdateHierarchyBranch();
	}
}

const GLuint GameObject::GetMaterialTexture(const int material_index) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == Component::ComponentType::MATERIAL)
		{
			ComponentMaterial* current_material = (ComponentMaterial*)components[i];
			if (current_material->index == material_index)
			{
				return current_material->GetTexture();
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

	transform.ShowComponentWindow();
	ImGui::Spacing();
	ImGui::Separator();
	aabb_collider.ShowComponentWindow();

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