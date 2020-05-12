#include "PanelInspector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleLight.h"


#include <Brofiler/Brofiler.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelInspector::PanelInspector()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_INFO_CIRCLE " Inspector";
}

void PanelInspector::Render()
{
	BROFILER_CATEGORY("Render Inspector Panel", Profiler::Color::BlueViolet);

	if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector", &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		if (App->editor->selected_game_object != nullptr)
		{
			gameobject_panel.Render(App->editor->selected_game_object);
		}

	}
	
	ImGui::Image((void *)App->cameras->directional_light_camera->depth_map, ImVec2(200, 200), ImVec2(0, 1),ImVec2(1, 0));
	ImGui::SliderFloat("Ortho camera width", &App->cameras->directional_light_camera->camera_frustum.orthographicWidth, 0, 100);
	ImGui::SliderFloat("Ortho camera height", &App->cameras->directional_light_camera->camera_frustum.orthographicHeight, 0, 100);

	//App->cameras->dir_light_game_object->transform.SetTranslation(float3(App->cameras->xpos, 0, 0));
	ImGui::End();
}