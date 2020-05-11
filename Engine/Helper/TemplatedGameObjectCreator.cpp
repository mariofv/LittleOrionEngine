#include "TemplatedGameObjectCreator.h"

#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

GameObject* TemplatedGameObjectCreator::CreatePrimitive(CoreResource resource_type)
{
	GameObject* primitive_game_object = App->scene->CreateGameObject();
	primitive_game_object->name = GetCoreResourceName(resource_type);

	ComponentMeshRenderer* mesh_renderer_component = (ComponentMeshRenderer*)primitive_game_object->CreateComponent(Component::ComponentType::MESH_RENDERER);
	mesh_renderer_component->SetMesh((uint32_t)resource_type);
	primitive_game_object->Update();

	//UndoRedo
	App->actions->action_game_object = primitive_game_object;
	App->actions->AddUndoAction(ModuleActions::UndoActionType::ADD_GAMEOBJECT);

	return primitive_game_object;
}

GameObject* TemplatedGameObjectCreator::CreateUIElement(Component::ComponentType ui_type)
{
	GameObject* main_canvas_game_object = App->ui->GetMainCanvasGameObject();

	if (main_canvas_game_object == nullptr)
	{
		main_canvas_game_object = App->scene->CreateGameObject();
		main_canvas_game_object->name = "Canvas";
		main_canvas_game_object->CreateComponent(Component::ComponentType::CANVAS);
	}

	GameObject* created_ui_element = App->scene->CreateGameObject();
	created_ui_element->name = Component::GetComponentTypeName(ui_type);
	created_ui_element->CreateComponent(ui_type);

	main_canvas_game_object->AddChild(created_ui_element);

	return created_ui_element;
}

std::string TemplatedGameObjectCreator::GetCoreResourceName(CoreResource resource_type)
{
	switch (resource_type)
	{
	case CoreResource::CUBE:
		return "Cube";
	case CoreResource::SPHERE:
		return "Sphere";
	case CoreResource::TORUS:
		return "Torus";
	case CoreResource::CYLINDER:
		return "Cylinder";
	case CoreResource::QUAD:
		return "Quad";
	}

	return "";
}