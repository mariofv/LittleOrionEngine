#include "PlayerAttack.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


PlayerAttack* PlayerAttackDLL()
{
	PlayerAttack* instance = new PlayerAttack();
	return instance;
}

PlayerAttack::PlayerAttack()
{
	panel = new PanelComponent();
}
// Use this for initialization before Start()
void PlayerAttack::Awake()
{	animation = (ComponentAnimation*) owner->GetComponent(Component::ComponentType::ANIMATION);

}
// Use this for initialization
void PlayerAttack::Start()
{
}
// Update is called once per frame
bool PlayerAttack::Attack()
{	if(App->input->GetGameInputDown("Punch"))
	{
		animation->ActiveAnimation("punch");
	}
	else if(App->input->GetGameInputDown("Kick"))
	{
		animation->ActiveAnimation("kick");
	}

	return animation->IsOnState("Punch") || animation->IsOnState("Kick");
}
// Use this for showing variables on inspector
void PlayerAttack::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}
//Use this for linking JUST GO automatically 
void PlayerAttack::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}