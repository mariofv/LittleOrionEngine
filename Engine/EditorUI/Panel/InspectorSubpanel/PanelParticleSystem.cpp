#include "PanelParticleSystem.h"

#include "Component/ComponentBillboard.h"
#include "Component/ComponentParticleSystem.h"
#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelParticleSystem::PanelParticleSystem()
{
	enabled = true;
	opened = true;
	window_name = "ParticleSystem Inspector";
}

void PanelParticleSystem::Render(ComponentParticleSystem* particle_system)
{
	if (ImGui::CollapsingHeader(ICON_FA_SQUARE " Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Active", &particle_system->active);
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			App->actions->DeleteComponentUndo(particle_system);
		}
		if (ImGui::Button("Copy"))
		{
			App->actions->SetCopyComponent(particle_system);
		}
		if (ImGui::Button("Paste component as new"))
		{
			App->actions->PasteComponent(particle_system);
		}
		if (ImGui::Button("Paste component values"))
		{
			App->actions->PasteComponentValues(particle_system);
		}
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static int emit_count = 0;
			ImGui::InputInt("##Emit_amount", &emit_count);
			if (ImGui::Button("Emit"))
			{
				particle_system->Emit(emit_count);
			}
			if (ImGui::Button("Start"))
			{
				particle_system->Play();
			}
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
			{
				particle_system->Pause();
			}
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
			{
				particle_system->Stop();
			}
		}
		
		ImGui::BeginChild("###Particles", ImVec2(0.f, 300.f), true);

		float billboard_texture_size = 17.5f;
		void* display_image;
		if (particle_system->billboard->billboard_texture.get() != nullptr)
		{
			display_image = (void*)(intptr_t)particle_system->billboard->billboard_texture->opengl_texture;
		}
		ImGuiID element_id = ImGui::GetID(std::to_string(particle_system->billboard->billboard_texture->GetUUID()).c_str());
		if (ImGui::ImageButton(
			display_image,
			ImVec2(billboard_texture_size, billboard_texture_size),
			ImVec2(0, 1),
			ImVec2(1, 0),
			1,
			ImVec4(1.f, 1.f, 1.f, 1.f),
			ImVec4(1.f, 1.f, 1.f, 1.f)
		))
		{
			App->editor->popups->resource_selector_popup.ShowPanel(element_id, ResourceType::TEXTURE);
		}
		uint32_t dropped_texture_uuid = ImGui::ResourceDropper<Texture>();
		if (dropped_texture_uuid != 0)
		{
			particle_system->billboard->ChangeTexture(dropped_texture_uuid);
			modified_by_user = true;
		}

		uint32_t resource_selector_texture = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (resource_selector_texture != 0)
		{
			particle_system->billboard->ChangeTexture(resource_selector_texture);
			modified_by_user = true;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Particle System"))
		{
			ImGui::Spacing();

			ImGui::DragFloat("Lifetime (s)", &particle_system->particles_life_time, 1.0F, 0.0F, 100.0F);
			ImGui::DragInt("Max particles", &particle_system->max_particles_number, 1.f, 0, MAX_PARTICLES);
			
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Size");

			int random_size = particle_system->size_random ? 1 : 0;
			if (ImGui::Combo("Initial Size Type", &random_size, "Constant\0Random\0"))
			{
				particle_system->size_random = random_size == 1;	
			}

			if (particle_system->size_random)
			{
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				ImGui::DragFloat("Initial Min size", &particle_system->min_size_of_particle, 0.1f, 0, 999);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				ImGui::DragFloat("Initial Max size", &particle_system->max_size_of_particle, 0.1f, 0, 999);
			}
			else
			{
				ImGui::DragFloat2("Initial Size", particle_system->particles_size.ptr(), 0.1f, 0, 999);
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Velocity");
			ImGui::DragFloat("Initial Velocity", &particle_system->velocity_particles_start, 0.01f, 0.0f, 100.0F);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::DragFloat("Gravity Modifier", &particle_system->gravity_modifier, 0.01f, 0.0f);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Checkbox("Local Space", &particle_system->follow_owner);
			ImGui::Spacing();

		}

		if (ImGui::CollapsingHeader("Emission"))
		{
			ImGui::Spacing();
			ImGui::DragFloat("Time Between Particles", &particle_system->time_between_particles, 0.1F, 0.0F, 10.0f);
			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Shape"))
		{
			ImGui::Spacing();

			int current_shape = static_cast<int>(particle_system->type_of_particle_system);
			if (ImGui::Combo("Shape###Combo", &current_shape, "Sphere\0Box\0Cone\0"))
			{
				switch (current_shape)
				{
				case 0:
					particle_system->type_of_particle_system = ComponentParticleSystem::TypeOfParticleSystem::SPHERE;
					break;
				case 1:
					particle_system->type_of_particle_system = ComponentParticleSystem::TypeOfParticleSystem::BOX;
					break;
				case 2:
					particle_system->type_of_particle_system = ComponentParticleSystem::TypeOfParticleSystem::CONE;
					break;
				}
			}

			if (particle_system->type_of_particle_system == ComponentParticleSystem::TypeOfParticleSystem::BOX)
			{
				if (particle_system->enabled_random_x)
				{
					ImGui::DragInt("Max X range", &particle_system->max_range_random_x, 1.0F, 0, 10000);
					ImGui::DragInt("Min X range", &particle_system->min_range_random_x, 1.0F, -10000, 0);
				}
				else
				{
					ImGui::DragInt("X position", &particle_system->position_x, 1.0F, -100, 10000);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Rand X", &particle_system->enabled_random_x);

				ImGui::Spacing();

				if (particle_system->enabled_random_z)
				{
					ImGui::DragInt("Max Z range", &particle_system->max_range_random_z, 1.0F, 0, 10000);
					ImGui::DragInt("Min Z range", &particle_system->min_range_random_z, 1.0F, -10000, 0);
				}
				else
				{
					ImGui::DragInt("Z position", &particle_system->position_z, 1.0F, -100, 10000);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Rand Z", &particle_system->enabled_random_z);

			}
			if (particle_system->type_of_particle_system == ComponentParticleSystem::TypeOfParticleSystem::CONE)
			{
				ImGui::DragFloat("Outer radius", &particle_system->outer_radius, 0.1F, 0.1F, 10);
				ImGui::DragFloat("Inner radius", &particle_system->inner_radius, 0.1F, 0.1F, 10);
			}
		}

		//Color of Particles

		if (ImGui::CollapsingHeader("Color"))
		{
			ImGui::ColorEdit4("Particle Color##2f", particle_system->initial_color.ptr(), ImGuiColorEditFlags_Float);
			ImGui::Text("Fade Between Colors");
			ImGui::Checkbox("###Fade Between Colors", &particle_system->fade_between_colors);
			ImGui::SameLine();
			if (!particle_system->fade_between_colors)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::ColorEdit4("Particle Color To Fade##2f", particle_system->color_to_fade.ptr(), ImGuiColorEditFlags_Float);
			ImGui::DragFloat("Color Fade time", &particle_system->color_fade_time, 0.01f, 0.0f, 10.0F);
			if (!particle_system->fade_between_colors)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		//Velocity over time
		ImGui::Checkbox("###Velocity over lifetime", &particle_system->velocity_over_time);
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Velocity Over Time"))
		{
			if (!particle_system->velocity_over_time)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			int velocity_type = static_cast<int>(particle_system->type_of_velocity_over_time);
			if (ImGui::Combo("Speed", &velocity_type, "Constant\0Linear\0Random Between Two Constants\0"))
			{
				switch (velocity_type)
				{
				case 0:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::CONSTANT;
					break;
				case 1:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::LINEAR;
					break;
				case 2:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::RANDOM_BETWEEN_TWO_CONSTANTS;
					break;
				}
			}

			switch (velocity_type)
			{
			case ComponentParticleSystem::TypeOfVelocityOverTime::CONSTANT:
				particle_system->velocity_over_time_speed_modifier;
				ImGui::DragFloat("Velocity Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				break;
			case ComponentParticleSystem::TypeOfVelocityOverTime::LINEAR:
				ImGui::DragFloat("Start Velocity Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				ImGui::DragFloat("End Velocity Modifier", &particle_system->velocity_over_time_speed_modifier_second, 0.01F);
				break;
			case ComponentParticleSystem::TypeOfVelocityOverTime::RANDOM_BETWEEN_TWO_CONSTANTS:
				particle_system->velocity_over_time_speed_modifier;
				ImGui::DragFloat("Min Velocity Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				ImGui::DragFloat("Max Velocity Modifier", &particle_system->velocity_over_time_speed_modifier_second, 0.01F);
				break;
			}

			if (!particle_system->velocity_over_time)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}

		}

		ImGui::Checkbox("###Orbit", &particle_system->orbit);
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Orbit"))
		{
			ImGui::Text("WIP");
		}
		ImGui::Checkbox("###Fade", &particle_system->fade);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Alpha Over Lifetime"))
		{
			if (!particle_system->fade)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			ImGui::DragFloat("Fade time", &particle_system->fade_time, 0.01f, 0.0f, 10.0F);
			if (!particle_system->fade)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}

		ImGui::Checkbox("###Size change over time", &particle_system->change_size);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Size Over Lifetime"))
		{
			if (!particle_system->change_size)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
		
			
			ImGui::Spacing();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
			ImGui::DragFloat("Min size", &particle_system->min_size_of_particle, 0.1f, 0, 999);
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
			ImGui::DragFloat("Max size", &particle_system->max_size_of_particle, 0.1f, 0, 999);
			
			ImGui::DragFloat("Speed", &particle_system->size_change_speed, 0.01f, 0.0f, 10.0F);

			if (!particle_system->change_size)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}

		ImGui::Checkbox("###Texture Sheet Animation", &particle_system->billboard->is_spritesheet);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Texture Sheet Animation"))
		{
			if (!particle_system->billboard->is_spritesheet)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::Checkbox("Tile random", &particle_system->tile_random);
			if (particle_system->tile_random)
			{
				ImGui::InputFloat("Max", &particle_system->max_tile_value);
				ImGui::InputFloat("Min", &particle_system->min_tile_value);
			}
			ImGui::DragInt("Columns", &particle_system->billboard->num_sprisheet_columns);
			ImGui::DragInt("Rows", &particle_system->billboard->num_sprisheet_rows);
			
			if (!particle_system->billboard->is_spritesheet)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		if (ImGui::CollapsingHeader("Renderer"))
		{
			int alignment_type = static_cast<int>(particle_system->billboard->alignment_type);
			if (ImGui::Combo("Billboard type", &alignment_type, "World\0View point\0Axial")) {
				switch (alignment_type)
				{
				case 0:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::WORLD);
					break;
				case 1:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::VIEW_POINT);
					break;
				case 2:
					particle_system->billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::AXIAL);
					break;
				}
			}

			
		}	
		ImGui::EndChild();
	}
}