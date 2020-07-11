#include "PanelStateMachine.h"

#include "Animation/AnimController.h"

#include "EditorUI/Helper/ImGuiHelper.h"

#include "Filesystem/File.h"

#include "Main/Application.h"

#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleAnimation.h"

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
	modified_by_user = false;
	//initialize GO to get corresponding animation controller
	if (App->editor->selected_game_object)
	{
		ComponentAnimation* animation_component = (ComponentAnimation*)App->editor->selected_game_object->GetComponent(Component::ComponentType::ANIMATION);

		AnimController* controller = animation_component ? animation_component->GetAnimController() : nullptr;
		bool valid = IsElegibleStateMachine(controller);
		animation_controller = valid ? animation_component->GetAnimController() : animation_controller_in_hierarchy;
	}
	else
	{
		animation_controller = animation_controller_in_hierarchy;
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
	if (modified_by_user)
	{
		App->resources->Save<StateMachine>(state_machine);
	}
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
		Widgets::Icon(node->Size/5, ax::Drawing::IconType::Diamond, true, ImVec4(255, 127, 80, 255), ImVec4(255, 127, 80, 255));
		ax::NodeEditor::EndPin();
		ImGui::EndGroup();

		ImGui::SameLine(node->Size.x - 1.5);

		ImGui::BeginGroup();
		ax::NodeEditor::BeginPin(node->output, ax::NodeEditor::PinKind::Output);
		Widgets::Icon(node->Size / 5, ax::Drawing::IconType::Flow, true, ImVec4(0, 0, 255, 255), ImVec4(0, 0, 255, 255));
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
			ImGui::PopID();
		}
		ImGui::Separator();
		if (ImGui::Button("Save"))
		{
			modified_by_user = true;
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

	for (auto & animation_component : App->animations->animations)
	{
		AnimController* controller = animation_component->GetAnimController();
		if (IsElegibleStateMachine(controller))
		{
			animation_controller_in_hierarchy = animation_component->GetAnimController();
			break;
		}
	}
}


