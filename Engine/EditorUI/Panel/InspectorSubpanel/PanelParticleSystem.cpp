#include "PanelParticleSystem.h"

#include "Component/ComponentBillboard.h"
#include "Component/ComponentParticleSystem.h"
#include "EditorUI/Helper/ImGuiHelper.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PopupsPanel/PanelPopupResourceSelector.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "EditorUI/Helper/BezierCurveDraw.hpp"

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
			return;
		}
		if (ImGui::Button("Copy"))
		{
			App->actions->SetCopyComponent(particle_system);
		}
		if (ImGui::Button("Paste component as new"))
		{
			App->actions->PasteComponent(particle_system->owner);
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
			if (ImGui::Button("Play"))
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
		ImGuiID element_id = ImGui::GetID(std::to_string(particle_system->billboard->UUID).c_str());
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
			particle_system->modified_by_user = true;
		}

		uint32_t resource_selector_texture = App->editor->popups->resource_selector_popup.GetSelectedResource(element_id);
		if (resource_selector_texture != 0)
		{
			particle_system->billboard->ChangeTexture(resource_selector_texture);
			particle_system->modified_by_user = true;
		}
		ImGui::SameLine();

		if (ImGui::CollapsingHeader("Particle System"))
		{
			ImGui::Spacing();

			particle_system->modified_by_user |= ImGui::DragFloat("Lifetime (s)", &particle_system->particles_life_time, 1.0F, 0.0F, 100.0F);
			particle_system->modified_by_user |= ImGui::DragInt("Max particles", &particle_system->max_particles_number, 1.f, 0, MAX_PARTICLES);
			
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Size");
			ImGui::DragFloat2("Initial Size", particle_system->particles_size.ptr(), 0.1f, 0, 999);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "Velocity");
			particle_system->modified_by_user |= ImGui::DragFloat("Initial Velocity", &particle_system->velocity_particles_start, 0.01f, 0.0f, 100.0F);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			particle_system->modified_by_user |= ImGui::DragFloat("Gravity Modifier", &particle_system->gravity_modifier, 0.01f, 0.0f);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			particle_system->modified_by_user |= ImGui::Checkbox("Local Space", &particle_system->follow_owner);
			ImGui::Spacing();

		}

		if (ImGui::CollapsingHeader("Emission"))
		{
			ImGui::Spacing();
			particle_system->modified_by_user |= ImGui::DragFloat("Time Between Particles", &particle_system->time_between_particles, 0.001F, 0.00001F, 10.0f, "%.5f");
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
				particle_system->modified_by_user = true;
			}

			if (particle_system->type_of_particle_system == ComponentParticleSystem::TypeOfParticleSystem::BOX)
			{
				if (particle_system->enabled_random_x)
				{
					particle_system->modified_by_user |= ImGui::DragInt("Max X range", &particle_system->max_range_random_x, 1.0F, 0, 10000);
					particle_system->modified_by_user |= ImGui::DragInt("Min X range", &particle_system->min_range_random_x, 1.0F, -10000, 0);
				}
				else
				{
					particle_system->modified_by_user |= ImGui::DragInt("X position", &particle_system->position_x, 1.0F, -100, 10000);
				}
				ImGui::SameLine();
				particle_system->modified_by_user |= ImGui::Checkbox("Rand X", &particle_system->enabled_random_x);

				ImGui::Spacing();

				if (particle_system->enabled_random_z)
				{
					particle_system->modified_by_user |= ImGui::DragInt("Max Z range", &particle_system->max_range_random_z, 1.0F, 0, 10000);
					particle_system->modified_by_user |= ImGui::DragInt("Min Z range", &particle_system->min_range_random_z, 1.0F, -10000, 0);
				}
				else
				{
					particle_system->modified_by_user |= ImGui::DragInt("Z position", &particle_system->position_z, 1.0F, -100, 10000);
				}
				ImGui::SameLine();
				particle_system->modified_by_user |= ImGui::Checkbox("Rand Z", &particle_system->enabled_random_z);

			}
			if (particle_system->type_of_particle_system == ComponentParticleSystem::TypeOfParticleSystem::CONE)
			{
				particle_system->modified_by_user |= ImGui::DragFloat("Outer radius", &particle_system->outer_radius, 0.1F, 0.1F, 10);
				particle_system->modified_by_user |= ImGui::DragFloat("Inner radius", &particle_system->inner_radius, 0.1F, 0.1F, 10);
			}
		}

		//Color of Particles
		if (ImGui::CollapsingHeader("Color"))
		{
			particle_system->modified_by_user |= ImGui::ColorEdit4("Particle Color##2f", particle_system->initial_color.ptr(), ImGuiColorEditFlags_Float);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			int color_fade_type = static_cast<int>(particle_system->type_of_color_change);
			if (ImGui::Combo("Fade Between Colors", &color_fade_type, "None\0Linear\0Curve\0"))
			{
				switch (color_fade_type)
				{
				case 0:
					particle_system->type_of_color_change = ComponentParticleSystem::TypeOfSizeColorChange::COLOR_NONE;
					break;
				case 1:
					particle_system->type_of_color_change = ComponentParticleSystem::TypeOfSizeColorChange::COLOR_LINEAR;
					break;
				case 2:
					particle_system->type_of_color_change = ComponentParticleSystem::TypeOfSizeColorChange::COLOR_CURVE;
					break;
				}
				particle_system->modified_by_user = true;
			}

			ImGui::Spacing();
			switch (color_fade_type)
			{
			case ComponentParticleSystem::TypeOfSizeColorChange::COLOR_NONE:
				break;
			case ComponentParticleSystem::TypeOfSizeColorChange::COLOR_LINEAR:
				particle_system->modified_by_user |= ImGui::ColorEdit4("Particle Color To Fade##2f", particle_system->color_to_fade.ptr(), ImGuiColorEditFlags_Float);
				particle_system->modified_by_user |= ImGui::DragFloat("Color Fade time", &particle_system->color_fade_time, 0.01f, 0.0f, particle_system->particles_life_time);
				break;
			case ComponentParticleSystem::TypeOfSizeColorChange::COLOR_CURVE:
				particle_system->modified_by_user |= ImGui::ColorEdit4("Particle Color To Fade##2f", particle_system->color_to_fade.ptr(), ImGuiColorEditFlags_Float);
				particle_system->modified_by_user |= ImGui::DrawBezierCubic(&particle_system->color_curve, ImVec2(0,1));
				break;
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

			ImGui::Spacing();
			int velocity_type = static_cast<int>(particle_system->type_of_velocity_over_time);
			if (ImGui::Combo("Speed", &velocity_type, "Constant\0Linear\0Random Between Two Constants\0Curve\0"))
			{
				switch (velocity_type)
				{
				case 0:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::VEL_CONSTANT;
					break;
				case 1:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::VEL_LINEAR;
					break;
				case 2:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::VEL_RANDOM_BETWEEN_TWO_CONSTANTS;
					break;
				case 3:
					particle_system->type_of_velocity_over_time = ComponentParticleSystem::TypeOfVelocityOverTime::VEL_CURVE;
					break;
				}
				particle_system->modified_by_user = true;
			}

			switch (velocity_type)
			{
			case ComponentParticleSystem::TypeOfVelocityOverTime::VEL_CONSTANT:
				particle_system->modified_by_user |= ImGui::DragFloat("Velocity Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				break;
			case ComponentParticleSystem::TypeOfVelocityOverTime::VEL_LINEAR:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Start Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("End Modifier", &particle_system->velocity_over_time_speed_modifier_second, 0.01F);
				break;
			case ComponentParticleSystem::TypeOfVelocityOverTime::VEL_RANDOM_BETWEEN_TWO_CONSTANTS:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Min Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Max Modifier", &particle_system->velocity_over_time_speed_modifier_second, 0.01F);
				break;
			case ComponentParticleSystem::TypeOfVelocityOverTime::VEL_CURVE:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Min Modifier", &particle_system->velocity_over_time_speed_modifier, 0.01F);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Max Modifier", &particle_system->velocity_over_time_speed_modifier_second, 0.01F);
				particle_system->modified_by_user |= ImGui::DrawBezierCubic(&particle_system->vel_curve, ImVec2(particle_system->velocity_over_time_speed_modifier, particle_system->velocity_over_time_speed_modifier_second));
				break;
			}
			ImGui::Spacing();

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
		particle_system->modified_by_user |= ImGui::Checkbox("###Fade", &particle_system->fade);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Alpha Over Lifetime"))
		{
			if (!particle_system->fade)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			particle_system->modified_by_user |= ImGui::DragFloat("Fade time", &particle_system->fade_time, 0.01f, 0.0f, 10.0F);
			if (!particle_system->fade)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}

		particle_system->modified_by_user |= ImGui::Checkbox("###Size change over time", &particle_system->size_over_time);
		ImGui::SameLine();
		if (ImGui::CollapsingHeader("Size Over Lifetime"))
		{
			if (!particle_system->size_over_time)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			ImGui::Spacing();
			int size_type = static_cast<int>(particle_system->type_of_size_over_time);
			if (ImGui::Combo("Size", &size_type, "Linear\0Random Between Two Constants\0Curve\0"))
			{
				switch (size_type)
				{
				case 0:
					particle_system->type_of_size_over_time = ComponentParticleSystem::TypeOfSizeOverTime::SIZE_LINEAR;
					break;
				case 1:
					particle_system->type_of_size_over_time = ComponentParticleSystem::TypeOfSizeOverTime::SIZE_RANDOM_BETWEEN_TWO_CONSTANTS;
					break;
				case 2:
					particle_system->type_of_size_over_time = ComponentParticleSystem::TypeOfSizeOverTime::SIZE_CURVE;
					break;
				}
				particle_system->modified_by_user = true;
			}

			switch (size_type)
			{
			case ComponentParticleSystem::TypeOfSizeOverTime::SIZE_LINEAR:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Min Modifier", &particle_system->min_size_of_particle, 0.1f, 0, 999);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Max Modifier", &particle_system->max_size_of_particle, 0.1f, 0, 999);
				break;
			case ComponentParticleSystem::TypeOfSizeOverTime::SIZE_RANDOM_BETWEEN_TWO_CONSTANTS:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Min Modifier", &particle_system->min_size_of_particle, 0.1f, 0, 999);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Max Modifier", &particle_system->max_size_of_particle, 0.1f, 0, 999);
				break;
			case ComponentParticleSystem::TypeOfSizeOverTime::SIZE_CURVE:
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Min Modifier", &particle_system->min_size_of_particle, 0.1f, 0, 999);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 5);
				particle_system->modified_by_user |= ImGui::DragFloat("Max Modifier", &particle_system->max_size_of_particle, 0.1f, 0, 999);
				particle_system->modified_by_user |= ImGui::DrawBezierCubic(&particle_system->size_curve, ImVec2(particle_system->min_size_of_particle, particle_system->max_size_of_particle));
				break;
			}
			ImGui::Spacing();
			
			if (!particle_system->size_over_time)
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

			particle_system->modified_by_user |= ImGui::Checkbox("Tile random", &particle_system->tile_random);
			if (particle_system->tile_random)
			{
				particle_system->modified_by_user |= ImGui::InputFloat("Max", &particle_system->max_tile_value);
				particle_system->modified_by_user |= ImGui::InputFloat("Min", &particle_system->min_tile_value);
			}
			particle_system->modified_by_user |= ImGui::DragInt("Columns", &particle_system->billboard->num_sprisheet_columns);
			particle_system->modified_by_user |= ImGui::DragInt("Rows", &particle_system->billboard->num_sprisheet_rows);
			
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
				particle_system->modified_by_user = true;
			}

			
		}	
		ImGui::EndChild();
	}
}