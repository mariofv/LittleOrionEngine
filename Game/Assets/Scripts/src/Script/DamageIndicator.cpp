#include "DamageIndicator.h"

#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


DamageIndicator* DamageIndicatorDLL()
{
	DamageIndicator* instance = new DamageIndicator();
	return instance;
}

DamageIndicator::DamageIndicator()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void DamageIndicator::Awake()
{
	damage_indicator_text = static_cast<ComponentText*>(owner->GetComponent(Component::ComponentType::UI_TEXT));
}

// Use this for initialization
void DamageIndicator::Start()
{

}

// Update is called once per frame
void DamageIndicator::Update()
{
	if (alive)
	{
		float elapsed_time = App->time->delta_time;
		current_time += elapsed_time;
		if (current_time > duration)
		{
			static_cast<ComponentText*>(owner->GetComponent(Component::ComponentType::CANVAS_RENDERER))->Disable();
			alive = false;
			return;
		}

		float alpha = 1 - current_time / duration;
		float4 font_color = damage_indicator_text->GetFontColor();
		damage_indicator_text->SetFontColor(float4(font_color.xyz(), alpha));
		owner->transform_2d.Translate(float3::unitY * speed * elapsed_time);
	}

}

bool DamageIndicator::IsAlive() const
{
	return alive;
}

void DamageIndicator::Spawn(int number, float2 position)
{
	alive = true;
	damage_indicator_text->SetText(std::to_string(number));
	position.y += offset_y;
	owner->transform_2d.SetTranslation(float3(position,0.f));
	static_cast<ComponentText*>(owner->GetComponent(Component::ComponentType::CANVAS_RENDERER))->Enable();

	current_time = 0.f;
}

// Use this for showing variables on inspector
void DamageIndicator::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::InputFloat("Speed", &speed);
	ImGui::InputFloat("Duration", &duration);
	ImGui::InputFloat("Offset Y", &offset_y);
}

//Use this for linking JUST GO automatically 
void DamageIndicator::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	//public_gameobjects.push_back(&example);
	//variable_names.push_back(GET_VARIABLE_NAME(example));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
void DamageIndicator::Save(Config& config) const
{
	config.AddFloat(speed, "Speed");
	config.AddFloat(duration, "Duration");
	config.AddFloat(offset_y, "Offset Y");
	Script::Save(config);
}

// //Use this for linking GO AND VARIABLES automatically
void DamageIndicator::Load(const Config& config)
{
	speed = config.GetFloat("Speed", 0.5f);
	duration = config.GetFloat("Duration", 1000.f);
	offset_y = config.GetFloat("Offset Y", 40.f);
	Script::Load(config);
}