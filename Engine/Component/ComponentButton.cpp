#include "ComponentButton.h"

#include "Main/Application.h"
#include "Main/Gameobject.h"

#include "Component/ComponentText.h"

#include "Module/ModuleScene.h"

ComponentButton::ComponentButton() : ComponentUI(ComponentUI::UIType::BUTTON)
{
	InitData();
}

ComponentButton::ComponentButton(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::BUTTON)
{
	InitData();
}

void ComponentButton::InitData()
{
	GameObject* created_game_object = App->scene->CreateGameObject();
	created_game_object->name = "Text";
	owner->AddChild(created_game_object);
	created_game_object->CreateComponent(Component::ComponentType::UI, ComponentUI::UIType::TEXT);
}

void ComponentButton::Render(float4x4* projection)
{
	ComponentUI::Render(projection);
}

void ComponentButton::Delete()
{
	ComponentUI::Delete();
}


void ComponentButton::Save(Config& config) const
{
	ComponentUI::Save(config);
}

void ComponentButton::Load(const Config& config)
{
	ComponentUI::Load(config);
}
