#include "PanelStateMachine.h"

#include "Animation/AnimController.h"

#include "EditorUI/Helper/ImGuiHelper.h"

#include "Filesystem/File.h"

#include "Main/Application.h"

#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/Resources/StateMachine.h"

#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

namespace
{
	const float NODE_SLOT_RADIUS = 6.2f;
}
PanelStateMachine::PanelStateMachine()
{
	opened = false;
	enabled = true;
	window_name = ICON_FA_PROJECT_DIAGRAM " State Machine";
	editor_context = ax::NodeEditor::CreateEditor();
}

PanelStateMachine::~PanelStateMachine()
{
	if (nodes.size() > 0)
	{
		nodes.erase(nodes.begin(), nodes.end());
	}
	if (links.size() > 0)
	{
		links.erase(links.begin(), links.end());
	}
	ax::NodeEditor::DestroyEditor(editor_context);
}

void PanelStateMachine::Render()
{
	//initialize GO to get corresponding animation controller
	if (App->editor->selected_game_object)
	{
		ComponentAnimation* animation_component = (ComponentAnimation*)App->editor->selected_game_object->GetComponent(Component::ComponentType::ANIMATION);

		AnimController* controller = animation_component ? animation_component->GetAnimController() : nullptr;
		bool valid = IsElegibleStateMachine(controller);
		animation_controller = valid ? animation_component->GetAnimController() : GetHierarchyAnimation();
	}
	else
	{
		animation_controller = GetHierarchyAnimation();
	}
	if (ImGui::Begin(window_name.c_str(), &opened, ImGuiWindowFlags_MenuBar))
	{
			ax::NodeEditor::SetCurrentEditor(editor_context);
			{
				LeftPanel();
				ax::NodeEditor::Begin("My Editor");
				if (ax::NodeEditor::IsBackgroundClicked())
				{
					ax::NodeEditor::Suspend();
					ImGui::OpenPopup("Editor Menu");
					ax::NodeEditor::Resume();
				}
				RenderStates();
				HandleInteraction();
				CreateNodeMenu();
				ax::NodeEditor::End();
			}
			if (firstFrame)
			{
				ax::NodeEditor::NavigateToContent(0.0f);
				firstFrame = false;
			}
	}
	ImGui::End();
	//save each time user modifies smth
	if (modified_by_user)
	{
		App->resources->Save<StateMachine>(state_machine);
		modified_by_user = false;
	}
}
bool PanelStateMachine::IsElegibleStateMachine(AnimController * controller)
{
	return controller && controller->state_machine && controller->state_machine->GetUUID() == state_machine->GetUUID();
}
void PanelStateMachine::RenderStates()
{
	ImGuiIO& io = ImGui::GetIO();

	draw_list = ImGui::GetWindowDrawList();
	for (auto & node : nodes)
	{
		assert(!node->id.Invalid);
		ImGui::PushID(node->id.AsPointer());
		// Start drawing nodes.
		if (firstFrame)
		{
			ax::NodeEditor::SetNodePosition(node->id, node->position);
			if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				node->position = node->position + io.MouseDelta;
			}
		}
		bool is_active = false;
		//get the current running animation controller to verify playing clip and select corresponding node/link
		if (animation_controller)
		{
			for (auto & playing_clip : animation_controller->playing_clips)
			{
				if (playing_clip.clip)
				{
					is_active |= playing_clip.playing == true && playing_clip.clip->name_hash == node->state->clip->name_hash;
				}
			}
		}
		if (is_active)
		{
			ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
			draw_list->AddCircleFilled(node->position, NODE_SLOT_RADIUS, IM_COL32(255, 0, 0, 255), 12);
		}
		ax::NodeEditor::BeginNode(node->id);
		ImGui::PushItemWidth(node->Size.x);
		uint64_t old_hash = node->state->name_hash;
		
		ImGui::TextColored(ImVec4(255, 255, 255, 255), "State Name");
		ImGui::SameLine();
		if(ImGui::InputText("###Node name Input", &node->state->name))
		{
			node->state->name_hash = std::hash<std::string>{}(node->state->name);
			for (auto link : links)
			{
				if (link->transition->source_hash == old_hash)
				{
					link->transition->source_hash = node->state->name_hash;
				}
				if (link->transition->target_hash == old_hash)
				{
					link->transition->target_hash = node->state->name_hash;
				}
			}
			modified_by_user = true;
		}
		
		std::string clip_name = node->state->clip ? node->state->clip->name : "Clip";
		ImGui::Button(clip_name.c_str(), ImVec2(node->Size.x, 0));
		uint32_t incoming_animation_uuid = ImGui::ResourceDropper<Animation>();
		if (incoming_animation_uuid != 0)
		{
			state_machine->AddClipToState(node->state, incoming_animation_uuid);
		}
		if (node->state->clip)
		{
			modified_by_user |= ImGui::Checkbox("Loop", &node->state->clip->loop);
			ImGui::SameLine(node->Size.x/2 + 20);
		}
		bool is_default_state = node->state->name_hash == state_machine->default_state;
		if (ImGui::Checkbox("Default", &is_default_state))
		{
			state_machine->default_state = node->state->name_hash;
			modified_by_user = true;
		}
		
		if (ImGui::InputFloat("Speed", &node->speed, 0.1f, 0.0f, "%.2f", 0))
		{
			node->state->speed = node->speed;
			modified_by_user = true;
		}

		ImGui::BeginGroup();
		ax::NodeEditor::BeginPin(node->input, ax::NodeEditor::PinKind::Input);
		ax::Widgets::Icon(node->Size/5, ax::Drawing::IconType::Diamond, true, ImVec4(255, 127, 80, 255), ImVec4(255, 127, 80, 255));
		ax::NodeEditor::EndPin();
		ImGui::EndGroup();

		ImGui::SameLine(node->Size.x - 1.5);

		ImGui::BeginGroup();
		ax::NodeEditor::BeginPin(node->output, ax::NodeEditor::PinKind::Output);
		ax::Widgets::Icon(node->Size / 5, ax::Drawing::IconType::Flow, true, ImVec4(0, 0, 255, 255), ImVec4(0, 0, 255, 255));
		ax::NodeEditor::EndPin();
		ImGui::EndGroup();
	
		ax::NodeEditor::EndNode();
		if (is_active)
		{
			ax::NodeEditor::PopStyleColor(1);
		}

		selected_nodes = GetSelectedNodes();

		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			ax::NodeEditor::Suspend();
			ImGui::OpenPopup("Create State");
			ax::NodeEditor::Resume();
		}
		ImVec2 position = ax::NodeEditor::GetNodePosition(node->id);
		node->state->position = float2(position.x, position.y);
		ImGui::PopItemWidth();
		ImGui::PopID();
		
		for (auto& selected : selected_nodes)
		{
			if (node == selected)
			{
				draw_list->AddCircleFilled(node->position, NODE_SLOT_RADIUS, IM_COL32(0, 0, 255, 255), 12);
			}
		}
	}

	for (auto& link : links)
	{
		ax::NodeEditor::Link(link->id,  link->output_id, link->input_id);
		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			ax::NodeEditor::Suspend();
			ImGui::OpenPopup("Link Menu");
			ax::NodeEditor::Resume();
		}
		if (animation_controller && animation_controller->active_transition)
		{
			bool same_source = animation_controller->active_transition->source_hash == link->transition->source_hash;
			bool same_target = animation_controller->active_transition->target_hash == link->transition->target_hash;
			//for each animation clip that is currently playing, display circle on correspodning node->state
			if (same_source && same_target)
			{
				//for each transition currently on progress, animate the corresponding transition link
				ax::NodeEditor::Flow(link->id);
			}
		}
	}
}

void PanelStateMachine::HandleInteraction()
{
	InteractionCreation();
	InteractionDelete();
}
void PanelStateMachine::InteractionDelete()
{
	if (ax::NodeEditor::BeginDelete())
	{
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId))
		{
			if (ax::NodeEditor::AcceptDeletedItem())
			{
				for (auto& link : links)
				{
					if (link->id == deletedLinkId)
					{
						auto transitions_it = std::find(state_machine->transitions.begin(), state_machine->transitions.end(), link->transition);
						state_machine->transitions.erase(transitions_it);
						links.erase(&link);
						break;
					}
				}
			}
		}
		ax::NodeEditor::NodeId deletedNodeId;
		while (ax::NodeEditor::QueryDeletedNode(&deletedNodeId))
		{
			if (ax::NodeEditor::AcceptDeletedItem())
			{
				for (auto& node : nodes)
				{
					if (node->id == deletedNodeId)
					{
						state_machine->RemoveState(node->state);
						nodes.erase(&node);
						break;
					}
				}
			}
		}
	}
	ax::NodeEditor::EndDelete();
}
void PanelStateMachine::InteractionCreation()
{
	if (ax::NodeEditor::BeginCreate())
	{
		ax::NodeEditor::PinId end_pin_id, start_pin_id;
		if (ax::NodeEditor::QueryNewLink(&start_pin_id, &end_pin_id))
		{
			if (end_pin_id && start_pin_id)
			{
				if (ax::NodeEditor::AcceptNewItem())
				{
					// Since we accepted new link, lets add one to our list of links.
					std::shared_ptr<Transition> new_transition = std::make_shared<Transition>();
					std::string source_name;
					std::string target_name;
					for (auto node : nodes)
					{
						if (node->input == end_pin_id)
						{
							new_transition->target_hash = node->state->name_hash;
							target_name = node->state->name;
						}

						if (node->output == start_pin_id)
						{
							new_transition->source_hash = node->state->name_hash;
							source_name = node->state->name;
						}
					}
					state_machine->transitions.push_back(new_transition);
					links.push_back(new LinkInfo{ ax::NodeEditor::LinkId(uniqueid++) , start_pin_id, end_pin_id, target_name, source_name,new_transition });

					// Draw new link.
					ax::NodeEditor::Link(links.back()->id, links.back()->input_id, links.back()->output_id);
				
				}

			}
		}
		ax::NodeEditor::PinId new_node_id;
		if (ax::NodeEditor::QueryNewNode(&new_node_id))
		{
			if (ax::NodeEditor::AcceptNewItem())
			{
				ax::NodeEditor::Suspend();
				ImGui::OpenPopup("Editor Menu");
				ax::NodeEditor::Resume();
			}
		}
	}
	ax::NodeEditor::EndCreate();
}
void PanelStateMachine::CreateNodeMenu()
{
	ax::NodeEditor::Suspend();
	if (ImGui::BeginPopup("Editor Menu"))
	{
		if (ImGui::BeginMenu("Create State"))
		{
			if (ImGui::MenuItem("Empty"))
			{
				NodeInfo* node = new NodeInfo({ ax::NodeEditor::NodeId(uniqueid++) });
				node->state = std::make_shared<State>("New Node", nullptr);
				state_machine->states.push_back(node->state);
				node->input = uniqueid++;
				node->output = uniqueid++;
				nodes.push_back(std::move(node));
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}


std::vector<NodeInfo*> PanelStateMachine::GetSelectedNodes()
{
	std::vector<ax::NodeEditor::NodeId> selection;
	selection.resize(ax::NodeEditor::GetSelectedObjectCount());
	ax::NodeEditor::GetSelectedNodes(selection.data(), static_cast<int>(selection.size()));

	std::vector<NodeInfo*> selected_nodes;
	for (auto & node : nodes)
	{
		bool isSelected = std::find(selection.begin(), selection.end(), node->id) != selection.end();
		if (isSelected)
		{
			selected_nodes.push_back(node);
		}
	}
	return selected_nodes;
}

std::vector<LinkInfo*> PanelStateMachine::GetSelectedLinks()
{
	std::vector<ax::NodeEditor::LinkId> selection;
	selection.resize(ax::NodeEditor::GetSelectedObjectCount());
	ax::NodeEditor::GetSelectedLinks(selection.data(), static_cast<int>(selection.size()));

	std::vector<LinkInfo*> selected_links;
	for (auto & link : links)
	{
		bool isSelected = std::find(selection.begin(), selection.end(), link->id) != selection.end();
		if (isSelected)
		{
			selected_links.push_back(link);
		}
	}
	return selected_links;
}

void PanelStateMachine::EraseNameFromVariables(std::vector<std::string>& names, uint64_t name_to_delete_hash)
{
	for(auto it = names.begin(); it != names.end(); ++it)
	{
		uint64_t variable_hash = std::hash<std::string>{}(*it);
		if(variable_hash == name_to_delete_hash)
		{
			names.erase(it);
			return;
		}
	}
}

void PanelStateMachine::LeftPanel()
{
	float window_size = ImGui::GetWindowSize().x / 3;
	if (ImGui::BeginChild("Details", ImVec2(window_size, 0)))
	{
		ImGui::Separator();
		auto& links = GetSelectedLinks();
		for (auto & link : links)
		{
			/*ImGui::LabelText(link->source.c_str(),"Transition From:");
			ImGui::LabelText(link->target.c_str(), "Transition To:");*/
			ImGui::TextColored(ImVec4{255, 255, 255, 255}, "Transition:");
			ImGui::Text(link->source.c_str()); ImGui::SameLine(); ImGui::Text("->"); ImGui::SameLine(); ImGui::Text(link->target.c_str());
			ImGui::PushID(link->id.AsPointer());
			ImGui::Separator();
			ImGui::Text("Interpolation time: ");
			ImGui::InputScalar("###Interpolation", ImGuiDataType_U64, &(link->transition->interpolation_time)); ImGui::SameLine(); ImGui::Text("ms");
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("Conditions:");
			ImGui::Checkbox("Exit time", &(link->transition->automatic));
			if (link->transition->automatic)
			{
				ImGui::InputScalar("Priority", ImGuiDataType_U64, &(link->transition->priority));
			}
			ImGui::BulletText("Trigger Name:");
			ImGui::InputText("###Trigger Name", &(link->transition->trigger));

			ImGui::Separator();
			ImGui::Text("Float Conditions: ");
			size_t i = 0;
			for (auto& condition : link->transition->float_conditions)
			{
				std::string variable_id("###VariableChosen");
				variable_id += std::to_string(i);
				const float window_width = ImGui::GetWindowWidth();
				float combo_width = window_width * 0.40f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(variable_id.c_str(), state_machine->GetNameOfVariable(condition.name_hash_variable).c_str()))
				{
					for(auto variable : state_machine->float_variables_names)
					{
						if (ImGui::Selectable(variable.c_str()))
						{
							condition.name_hash_variable = std::hash<std::string>{}(variable);
						}
					}

					ImGui::EndCombo();
				}


				ImGui::SameLine();
				std::string comparator_id("###Comparator");
				comparator_id += std::to_string(i);
				size_t index = static_cast<size_t>(condition.comparator);
				combo_width = window_width * 0.25f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(comparator_id.c_str(), comparator_name[index]))
				{
					//Greater
					if (ImGui::Selectable(comparator_name[0]))
					{
						condition.comparator = Comparator::GREATER;
						condition.comparator_function = std::greater();
					}
					
					//Lesser
					if (ImGui::Selectable(comparator_name[1]))
					{
						condition.comparator = Comparator::LESSER;
						condition.comparator_function = std::less();
					}

					//Equal
					if (ImGui::Selectable(comparator_name[2]))
					{
						condition.comparator = Comparator::EQUAL;
						condition.comparator_function = std::equal_to();
					}

					//NotEqual
					if (ImGui::Selectable(comparator_name[3]))
					{
						condition.comparator = Comparator::NOT_EQUAL;
						condition.comparator_function = std::not_equal_to();
					}

					ImGui::EndCombo();
				}
				ImGui::SameLine();
				std::string x_id("###Value");
				x_id += std::to_string(i);
				combo_width = window_width * 0.20f;
				ImGui::SetNextItemWidth(combo_width);
				ImGui::DragFloat(x_id.c_str(), &condition.value, 0.01f, 0.f, 100.f);

				ImGui::SameLine();
				std::string delete_id("X###Deletethis");
				delete_id += std::to_string(i);
				combo_width = window_width * 0.15f;
				ImGui::SetNextItemWidth(combo_width);
				if(ImGui::Button(delete_id.c_str()))
				{
					link->transition->float_conditions.erase(link->transition->float_conditions.begin() + i);
					break;
				}

				++i;
			}

			ImGui::Separator();
		
			if (ImGui::Button("+###float"))
			{
				Condition<float> condition(state_machine->float_variables.begin()->first, std::greater(), 0.f);
				link->transition->float_conditions.push_back(condition);
			}


			//IntConditions
			ImGui::Text("Int Conditions: ");
			i = 0;
			for (auto& condition : link->transition->int_conditions)
			{
				std::string variable_id("###IntVariableChosen");
				variable_id += std::to_string(i);
				const float window_width = ImGui::GetWindowWidth();
				float combo_width = window_width * 0.40f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(variable_id.c_str(), state_machine->GetNameOfVariable(condition.name_hash_variable).c_str()))
				{
					for (auto variable : state_machine->int_variables_names)
					{
						if (ImGui::Selectable(variable.c_str()))
						{
							condition.name_hash_variable = std::hash<std::string>{}(variable);
						}
					}

					ImGui::EndCombo();
				}


				ImGui::SameLine();
				std::string comparator_id("###IntComparator");
				comparator_id += std::to_string(i);
				size_t index = static_cast<size_t>(condition.comparator);
				combo_width = window_width * 0.25f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(comparator_id.c_str(), comparator_name[index]))
				{
					//Greater
					if (ImGui::Selectable(comparator_name[0]))
					{
						condition.comparator = Comparator::GREATER;
						condition.comparator_function = std::greater();
					}

					//Lesser
					if (ImGui::Selectable(comparator_name[1]))
					{
						condition.comparator = Comparator::LESSER;
						condition.comparator_function = std::less();
					}

					//Equal
					if (ImGui::Selectable(comparator_name[2]))
					{
						condition.comparator = Comparator::EQUAL;
						condition.comparator_function = std::equal_to();
					}

					//NotEqual
					if (ImGui::Selectable(comparator_name[3]))
					{
						condition.comparator = Comparator::NOT_EQUAL;
						condition.comparator_function = std::not_equal_to();
					}

					ImGui::EndCombo();
				}
				ImGui::SameLine();
				std::string x_id("###IntValue");
				x_id += std::to_string(i);
				combo_width = window_width * 0.20f;
				ImGui::SetNextItemWidth(combo_width);
				ImGui::DragInt(x_id.c_str(), &condition.value, 0.01f, 0.f, 10000.f);

				ImGui::SameLine();
				std::string delete_id("X###IntDeletethis");
				delete_id += std::to_string(i);
				combo_width = window_width * 0.15f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::Button(delete_id.c_str()))
				{
					link->transition->int_conditions.erase(link->transition->int_conditions.begin() + i);
					break;
				}

				++i;
			}

			ImGui::Separator();

			if (ImGui::Button("+###int"))
			{
				Condition<int> condition(state_machine->int_variables.begin()->first, std::greater(), 0);
				link->transition->int_conditions.push_back(condition);
			}

			//BoolConditions
			ImGui::Text("Bool Conditions: ");
			i = 0;
			for (auto& condition : link->transition->bool_conditions)
			{
				std::string variable_id("###BoolVariableChosen");
				variable_id += std::to_string(i);
				const float window_width = ImGui::GetWindowWidth();
				float combo_width = window_width * 0.40f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(variable_id.c_str(), state_machine->GetNameOfVariable(condition.name_hash_variable).c_str()))
				{
					for (auto variable : state_machine->bool_variables_names)
					{
						if (ImGui::Selectable(variable.c_str()))
						{
							condition.name_hash_variable = std::hash<std::string>{}(variable);
						}
					}

					ImGui::EndCombo();
				}


				ImGui::SameLine();
				std::string comparator_id("###BoolComparator");
				comparator_id += std::to_string(i);
				size_t index = static_cast<size_t>(condition.comparator);
				combo_width = window_width * 0.25f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(comparator_id.c_str(), comparator_name[index]))
				{
					//Equal
					if (ImGui::Selectable(comparator_name[2]))
					{
						condition.comparator = Comparator::EQUAL;
						condition.comparator_function = std::equal_to();
					}

					//NotEqual
					if (ImGui::Selectable(comparator_name[3]))
					{
						condition.comparator = Comparator::NOT_EQUAL;
						condition.comparator_function = std::not_equal_to();
					}

					ImGui::EndCombo();
				}
				ImGui::SameLine();
				std::string x_id("###BoolValue");
				x_id += std::to_string(i);
				combo_width = window_width * 0.20f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::BeginCombo(x_id.c_str(), bool_conditions[condition.value]))
				{
					//false
					if (ImGui::Selectable(bool_conditions[0]))
					{
						condition.value = false;
					}

					//true
					if (ImGui::Selectable(bool_conditions[1]))
					{
						condition.value = true;
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();
				std::string delete_id("X###BoolDeletethis");
				delete_id += std::to_string(i);
				combo_width = window_width * 0.15f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::Button(delete_id.c_str()))
				{
					link->transition->bool_conditions.erase(link->transition->bool_conditions.begin() + i);
					break;
				}

				++i;
			}

			ImGui::Separator();

			if (ImGui::Button("+###bool"))
			{
				Condition<bool> condition(state_machine->bool_variables.begin()->first, std::equal_to(), false);
				condition.comparator = Comparator::EQUAL;
				link->transition->bool_conditions.push_back(condition);
			}

			ImGui::PopID();
		}


		
		ImGui::Separator();
		ImGui::Text("Parameters: ");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Floats");
		ImGui::Separator();
		size_t i = 0;
		for(auto float_variable : state_machine->float_variables)
		{
			ImGui::DragFloat(state_machine->GetNameOfVariable(float_variable.first).c_str(), &float_variable.second, 0.01f, 0.f, 100.f);
			state_machine->float_variables[float_variable.first] = float_variable.second;
			ImGui::SameLine();
			std::string x_id("X###float");
			x_id += std::to_string(i);
			if(ImGui::Button(x_id.c_str()))
			{
				state_machine->float_variables.erase(float_variable.first);
				EraseNameFromVariables(state_machine->float_variables_names, float_variable.first);
				break;
			}

			++i;
		}
		ImGui::Separator();
		ImGui::InputText("###Float Name", &float_auxiliar_variable);
		ImGui::SameLine();
		if(ImGui::Button("Add float variable"))
		{
			uint64_t name_hash = std::hash<std::string>{}(float_auxiliar_variable);
			if (state_machine->float_variables.find(name_hash) == state_machine->float_variables.end())
			{
				state_machine->float_variables[name_hash] = 0.f;
				state_machine->float_variables_names.push_back(float_auxiliar_variable);			
			}
		}

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ints");
		ImGui::Separator();
		i = 0;
		for (auto int_variable : state_machine->int_variables)
		{
			ImGui::DragInt(state_machine->GetNameOfVariable(int_variable.first).c_str(), &int_variable.second, 1, 0, 10000);
			state_machine->int_variables[int_variable.first] = int_variable.second;
			ImGui::SameLine();
			std::string x_id("X###Int");
			x_id += std::to_string(i);
			if (ImGui::Button(x_id.c_str()))
			{
				state_machine->int_variables.erase(int_variable.first);
				EraseNameFromVariables(state_machine->int_variables_names, int_variable.first);
				break;
			}

			++i;
		}
		ImGui::Separator();
		ImGui::InputText("###Int Name", &int_auxiliar_variable);
		ImGui::SameLine();
		if (ImGui::Button("Add int variable"))
		{
			uint64_t name_hash = std::hash<std::string>{}(int_auxiliar_variable);
			if (state_machine->int_variables.find(name_hash) == state_machine->int_variables.end())
			{
				state_machine->int_variables[name_hash] = 0;
				state_machine->int_variables_names.push_back(int_auxiliar_variable);			
			}
		}

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Bools");
		ImGui::Separator();
		i = 0;
		for (auto bool_variable : state_machine->bool_variables)
		{
			if (ImGui::BeginCombo(state_machine->GetNameOfVariable(bool_variable.first).c_str(), bool_conditions[bool_variable.second]))
			{
				//false
				if (ImGui::Selectable(bool_conditions[0]))
				{
					bool_variable.second = false;
				}

				//true
				if (ImGui::Selectable(bool_conditions[1]))
				{
					bool_variable.second = true;
				}

				ImGui::EndCombo();
			}
			
			state_machine->bool_variables[bool_variable.first] = bool_variable.second;
			ImGui::SameLine();
			std::string x_id("X###Bool");
			x_id += std::to_string(i);
			if (ImGui::Button(x_id.c_str()))
			{
				state_machine->bool_variables.erase(bool_variable.first);
				EraseNameFromVariables(state_machine->bool_variables_names, bool_variable.first);
				break;
			}

			++i;
		}
		ImGui::Separator();
		ImGui::InputText("###Bool Name", &bool_auxiliar_variable);
		ImGui::SameLine();
		if (ImGui::Button("Add bool variable"))
		{
			uint64_t name_hash = std::hash<std::string>{}(bool_auxiliar_variable);
			if(state_machine->bool_variables.find(name_hash) == state_machine->bool_variables.end())
			{
				state_machine->bool_variables[name_hash] = false;
				state_machine->bool_variables_names.push_back(bool_auxiliar_variable);			
			}
		}

		ImGui::Separator();
		if (ImGui::Button("Save"))
		{
			modified_by_user = false;
			App->resources->Save<StateMachine>(state_machine);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			OpenStateMachine(state_machine->GetUUID());
		}
		ImGui::Separator();
	}
	ImGui::EndChild();
	ImGui::SameLine(0.0f, 12.0f);
}

void PanelStateMachine::OpenStateMachine(uint32_t state_machine_uuid)
{
	nodes.clear();
	links.clear();

	state_machine = App->resources->Load<StateMachine>(state_machine_uuid);

	std::string original_file = App->resources->resource_DB->GetEntry(state_machine->GetUUID())->imported_file_path;
	Path* state_machine_json_path = App->filesystem->GetPath(original_file);
	FileData state_machine_data = state_machine_json_path->GetFile()->Load();

	char* state_machine_data_buffer = (char*)state_machine_data.buffer;
	std::string serialized_state_machine_string = std::string(state_machine_data_buffer, state_machine_data.size);

	state_machine->LoadNames(serialized_state_machine_string);
	//Tranform form state machine to ui
	for (auto & state : state_machine->states)
	{
		NodeInfo * node = new NodeInfo({ ax::NodeEditor::NodeId(uniqueid++) });
		node->state = state;
		node->output = uniqueid++;
		node->input = uniqueid++;
		node->position = ImVec2(state->position.x, state->position.y);
		node->speed = state->speed;
		nodes.push_back(node);
	}

	for (auto & link : state_machine->transitions)
	{
		ax::NodeEditor::PinId source;
		ax::NodeEditor::PinId target;
		std::string source_name;
		std::string target_name;
		for (auto & node : nodes)
		{
			if (node->state->name_hash == link->source_hash)
			{
				source = node->output;
				source_name = node->state->name;
			}
			if (node->state->name_hash == link->target_hash)
			{
				target = node->input;
				target_name = node->state->name;
			}
		}
		links.push_back(new LinkInfo{ ax::NodeEditor::LinkId(uniqueid++) , target, source, target_name, source_name, link });
	}
	delete[] state_machine_data.buffer;
	firstFrame = true;
}

AnimController* PanelStateMachine::GetHierarchyAnimation()
{
	for (auto & animation_component : App->animations->animations)
	{
		AnimController* controller = animation_component->GetAnimController();
		if (IsElegibleStateMachine(controller))
		{
			return animation_component->GetAnimController();
		}
	}
	return nullptr;
}


