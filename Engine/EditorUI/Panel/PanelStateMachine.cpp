#include "PanelStateMachine.h"
#include "Filesystem/File.h"

#include "EditorUI/Helper/ImGuiHelper.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/StateMachine.h"

#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <FontAwesome5/IconsFontAwesome5.h>

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
	if (ImGui::Begin(window_name.c_str(), &opened, ImGuiWindowFlags_MenuBar))
	{
			ax::NodeEditor::SetCurrentEditor(editor_context);
			{
				LeftPanel();
				ax::NodeEditor::Begin("My Editor");
				if (ImGui::IsMouseClicked(1))
				{
					ax::NodeEditor::Suspend();
					ImGui::OpenPopup("Editor Menu");
					ax::NodeEditor::Resume();
				}
				RenderStates();
				HandleInteraction();
				CreateNodeMenu();
			}
			ax::NodeEditor::End();
			if (firstFrame)
			{
				ax::NodeEditor::NavigateToContent(0.0f);
				firstFrame = false;
			}

	}
	ImGui::End();
}
void PanelStateMachine::RenderStates()
{
	ImVec2 position(10,10);
	for (auto & node : nodes)
	{
		assert(!node->id.Invalid);
		ImGui::PushID(node->id.AsPointer());
		// Start drawing nodes.
		if (firstFrame)
		{
			ax::NodeEditor::SetNodePosition(node->id, position);
		}
		ax::NodeEditor::BeginNode(node->id);
		ImGui::PushItemWidth(100.0f);
		uint64_t old_hash = node->state->name_hash;
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
		}

		std::string clip_name = node->state->clip ? node->state->clip->name : "Clip";
		ImGui::Button(clip_name.c_str());
		uint32_t incoming_animation_uuid = ImGui::ResourceDropper<Animation>();
		if (incoming_animation_uuid != 0)
		{
			state_machine->AddClipToState(node->state, incoming_animation_uuid);
		}

		if (node->state->clip)
		{
			ImGui::Checkbox("Loop", &node->state->clip->loop);
			ImGui::SameLine();
		}
		bool is_default_state = node->state->name_hash == state_machine->default_state;
		if (ImGui::Checkbox("Default", &is_default_state))
		{
			state_machine->default_state = node->state->name_hash;
		}
	
		ImGui::BeginGroup();
		ax::NodeEditor::BeginPin(node->input, ax::NodeEditor::PinKind::Input);
		ImGui::Text("-> In");
		ax::NodeEditor::EndPin();
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		ax::NodeEditor::BeginPin(node->output, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ImGui::EndGroup();
		ax::NodeEditor::EndNode();
		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			ax::NodeEditor::Suspend();
			ImGui::OpenPopup("Create State");
			ax::NodeEditor::Resume();
		}
		position = ax::NodeEditor::GetNodePosition(node->id);
		position.x+= ax::NodeEditor::GetNodeSize(node->id).x;

		ImGui::PopItemWidth();
		ImGui::PopID();

	}
	for (auto& link : links)
	{
		ax::NodeEditor::Link(link->id, link->input_id, link->output_id);
		if (ImGui::IsMouseClicked(1) && ImGui::IsItemHovered())
		{
			ax::NodeEditor::Suspend();
			ImGui::OpenPopup("Link Menu");
			ax::NodeEditor::Resume();
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
	if (ImGui::BeginChild("Details", ImVec2(300, 0)))
	{
		if (ImGui::Button("Save"))
		{
			App->resources->Save<StateMachine>(state_machine);
			//TODO: Change this to be saved after eachtime user modifies anything, see PanelMaterial for reference
		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			OpenStateMachine(state_machine->GetUUID());
		}
		ImGui::Separator();
		auto& links = GetSelectedLinks();
		for (auto & link : links)
		{
			ImGui::Text(link->source.c_str());ImGui::SameLine(); ImGui::Text("->");ImGui::SameLine(); ImGui::Text(link->target.c_str());
			ImGui::PushID(link->id.AsPointer());
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
			ImGui::Text("Trigger Name:");
			ImGui::InputText("###Trigger Name", &(link->transition->trigger));
			ImGui::PopID();
		}
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

	state_machine->Load(serialized_state_machine_string);
	//Tranform form state machine to ui
	for (auto & state : state_machine->states)
	{
		NodeInfo * node = new NodeInfo({ ax::NodeEditor::NodeId(uniqueid++) });
		node->state = state;
		node->output = uniqueid++;
		node->input = uniqueid++;
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
}
