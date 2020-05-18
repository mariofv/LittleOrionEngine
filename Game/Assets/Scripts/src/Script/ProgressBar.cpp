#include "ProgressBar.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


ProgressBar* ProgressBarDLL()
{
	ProgressBar* instance = new ProgressBar();
	return instance;
}

ProgressBar::ProgressBar()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void ProgressBar::Awake()
{
	fill_bar_transform = &fill_bar_game_object->transform_2d;
	progress_bar_transform = &owner->transform_2d;
}

// Use this for initialization
void ProgressBar::Start()
{
	SetProgress(progress);
}

// Update is called once per frame
void ProgressBar::Update()
{
}

void ProgressBar::SetProgress(float new_progress)
{
	if (new_progress < 0.f || new_progress > 1.f)
	{
		return;
	}

	progress = new_progress;
	float progress_bar_width = progress_bar_transform->GetWidth();
	float fill_bar_padding = progress_bar_width * (1 - progress);

	if (!inverted)
	{
		fill_bar_transform->SetRight(fill_bar_padding);
	}
	else
	{
		fill_bar_transform->SetLeft(fill_bar_padding);
	}
}

// Use this for showing variables on inspector
void ProgressBar::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Inverted", &inverted);
}

//Use this for linking JUST GO automatically 
void ProgressBar::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&fill_bar_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(fill_bar_game_object));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void ProgressBar::Save(Config& config) const
{
	config.AddBool(inverted, "Inverted");
	Script::Save(config);
}

void ProgressBar::Load(const Config& config)
{
	inverted = config.GetBool("Inverted", false);
	Script::Load(config);
}