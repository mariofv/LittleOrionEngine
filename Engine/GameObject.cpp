#include "GameObject.h"
#include "Application.h"
#include "Hierarchy.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTexture.h"
#include "Texture.h"
#include "Utils.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMaterial.h"
#include "Component/ComponentMesh.h"

#include "imgui.h"
#include "imgui_stdlib.h"
#include <FontAwesome5/IconsFontAwesome5.h>
#include <pcg_basic.h>

#include <algorithm>

GameObject::GameObject() : transform(this), aabb(this), UUID(pcg32_random())
{
}

GameObject::GameObject(const std::string name) :
	name(name),
	transform(this),
	aabb(this),
	UUID(pcg32_random())
{
}

GameObject::~GameObject()
{
	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

	for (int i = (components.size() - 1); i >= 0; --i)
	{
		components[i]->Delete();
	}
	components.clear();

	for (int i =  (children.size() - 1); i >= 0 ; --i)
	{
		children[i]->parent = nullptr;
		App->scene->RemoveGameObject(children[i]);
	}
	children.clear();
}

bool GameObject::IsEnabled() const
{
	return active;
}

void GameObject::SetStatic(bool is_static)
{

	SetHierarchyStatic(is_static);
	App->renderer->GenerateQuadTree();
}

void GameObject::SetHierarchyStatic(bool is_static)
{
	this->is_static = is_static;
	for (auto & child : children)
	{
		child->SetStatic(is_static);
	}
}

bool GameObject::IsStatic() const
{
	return is_static;
}
void GameObject::Update()
{
	transform.GenerateGlobalModelMatrix();
	aabb.GenerateBoundingBox();

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->Update();
	}

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Update();
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
	case Component::ComponentType::CAMERA:
		created_component = App->cameras->CreateComponentCamera();
		break;

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

void GameObject::RemoveComponent(Component * component_to_remove) 
{
	auto it = std::find(components.begin(), components.end(), component_to_remove);
	if (it != components.end()) 
	{
		component_to_remove->Delete();
		components.erase(it);
	}
}

Component*  GameObject::GetComponent(const Component::ComponentType type) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == type)
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

	ImGui::SameLine();
	if (ImGui::Checkbox("Static", &is_static))
	{
		SetStatic(is_static);
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	transform.ShowComponentWindow();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	aabb.ShowComponentWindow();


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