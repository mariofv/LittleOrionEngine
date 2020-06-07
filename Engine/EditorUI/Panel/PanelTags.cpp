#include "PanelTags.h"

#include "EditorUI/Panel/PanelPopups.h"
#include "Helper/TagManager.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>


PanelTags::PanelTags()
{
	window_name = ICON_FA_TAGS " Tags";
}

void PanelTags::Render()
{
	if(ImGui::Begin(window_name.c_str(), &opened))
	{
		ImGui::Text("Project Tags");
		for(auto& tag : App->editor->tag_manager->tags)
		{
			ImGui::Text("%s", tag.c_str());
			ImGui::SameLine();
			if(ImGui::Button("-"))
			{
				App->editor->tag_manager->RemoveTag(tag);
				break;
			}
		}

		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if(ImGui::Button("Add Tag"))
		{
			App->editor->popups->add_tag_popup_shown = true;
		}
	}
	ImGui::End();
}