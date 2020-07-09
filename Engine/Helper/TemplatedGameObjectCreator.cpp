#include "TemplatedGameObjectCreator.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentText.h"

#include "Main/Application.h"
#include "Module/ModuleActions.h"
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

GameObject* TemplatedGameObjectCreator::CreateUIImage()
{
	GameObject* main_canvas_game_object = App->ui->GetMainCanvasGameObject();

	if (main_canvas_game_object == nullptr)
	{
		main_canvas_game_object = CreateMainCanvas();
	}
	if (!App->ui->ExistEventSystem())
	{
		CreateEventSystem();
	}

	GameObject* created_ui_element = App->scene->CreateGameObject();
	created_ui_element->name = Component::GetComponentTypeName(Component::ComponentType::UI_IMAGE);
	created_ui_element->CreateComponent(Component::ComponentType::UI_IMAGE);

	main_canvas_game_object->AddChild(created_ui_element);

	return created_ui_element;
}

GameObject* TemplatedGameObjectCreator::CreateUIButton()
{
	GameObject* main_canvas_game_object = App->ui->GetMainCanvasGameObject();
 
	if (main_canvas_game_object == nullptr)
	{
		main_canvas_game_object = CreateMainCanvas();
	}
	if (!App->ui->ExistEventSystem())
	{
		CreateEventSystem();
	}

	GameObject* created_button = App->scene->CreateGameObject();
	created_button->name = Component::GetComponentTypeName(Component::ComponentType::UI_BUTTON);
	created_button->CreateComponent(Component::ComponentType::UI_IMAGE);
	created_button->CreateComponent(Component::ComponentType::UI_BUTTON);
	created_button->CreateComponent(Component::ComponentType::CANVAS_RENDERER);
	created_button->transform_2d.SetSize(float2(160, 30));

	GameObject* button_text = App->scene->CreateGameObject();
	button_text->name = Component::GetComponentTypeName(Component::ComponentType::UI_TEXT);
	created_button->AddChild(button_text);

	button_text->transform_2d.SetAnchorPreset(AnchorPreset::AnchorPresetType::STRETCH_HORIZONTAL_STRETCH_VERTICAL);
	button_text->transform_2d.SetBottom(0.f);
	button_text->transform_2d.SetTop(0.f);
	button_text->transform_2d.SetLeft(0.f);
	button_text->transform_2d.SetRight(0.f);

	ComponentText* component_text = static_cast<ComponentText*>(button_text->CreateComponent(Component::ComponentType::UI_TEXT));
	component_text->SetFont(static_cast<uint32_t>(CoreResource::DEFAULT_FONT));
	component_text->SetFontSize(12);
	component_text->SetText("Text");

	main_canvas_game_object->AddChild(created_button);

	return created_button;
}

GameObject* TemplatedGameObjectCreator::CreateUIText()
{
	GameObject* main_canvas_game_object = App->ui->GetMainCanvasGameObject();

	if (main_canvas_game_object == nullptr)
	{
		main_canvas_game_object = CreateMainCanvas();
	}
	if (!App->ui->ExistEventSystem())
	{
		CreateEventSystem();
	}

	GameObject* created_text = App->scene->CreateGameObject();
	created_text->name = Component::GetComponentTypeName(Component::ComponentType::UI_TEXT);
	created_text->transform_2d.SetSize(float2(160, 30));
	ComponentText* component_text = static_cast<ComponentText*>(created_text->CreateComponent(Component::ComponentType::UI_TEXT));
	component_text->SetFont((uint32_t)CoreResource::DEFAULT_FONT);
	component_text->SetFontSize(12);
	component_text->SetText("New Text");
	component_text->horizontal_alignment = ComponentText::HorizontalAlignment::CENTER;

	created_text->CreateComponent(Component::ComponentType::CANVAS_RENDERER);

	main_canvas_game_object->AddChild(created_text);

	return created_text;
}

GameObject* TemplatedGameObjectCreator::CreateMainCanvas()
{
	GameObject* main_canvas_game_object = App->scene->CreateGameObject();
	main_canvas_game_object->name = "Canvas";
	main_canvas_game_object->CreateComponent(Component::ComponentType::CANVAS);

	return main_canvas_game_object;
}

GameObject* TemplatedGameObjectCreator::CreateEventSystem()
{
	GameObject* event_system_game_object = App->scene->CreateGameObject();
	event_system_game_object->name = "Event System";
	event_system_game_object->CreateComponent(Component::ComponentType::EVENT_SYSTEM);

	return event_system_game_object;
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