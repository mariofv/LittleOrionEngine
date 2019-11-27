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
	transform->GenerateGlobalModelMatrix();

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->Update();
	}

}

void GameObject::Render() const
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
		children[i]->Render();
	}
}

void GameObject::AddChild(GameObject *child)
{
	if (child->parent != nullptr)
	{
		child->parent->RemoveChild(child);
	}
	
	child->parent = this;
	child->hierarchy_depth = hierarchy_depth + 1;

	child->transform->ChangeLocalSpace(transform->GetGlobalModelMatrix());
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

const GLuint GameObject::GetMaterialTexture(const int material_index) const
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