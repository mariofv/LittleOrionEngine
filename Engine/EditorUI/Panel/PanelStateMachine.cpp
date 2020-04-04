#include "PanelStateMachine.h"
#include "Filesystem/File.h"
#include <FontAwesome5/IconsFontAwesome5.h>

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/StateMachine.h"

#include "imgui_internal.h"
#include <imgui_stdlib.h>
PanelStateMachine::PanelStateMachine()
{
	opened = false;
	enabled = true;
	window_name = ICON_FA_PROJECT_DIAGRAM " State Machine";
	editor_context = ax::NodeEditor::CreateEditor();
}

PanelStateMachine::~PanelStateMachine()
{
	nodes.erase(nodes.begin(), nodes.end());
	if (links.size() > 0)
	{
		links.erase(links.begin(), links.end());
	}
	ax::NodeEditor::DestroyEditor(editor_context);
	state_machine->Save();
}

void PanelStateMachine::Render()
{
	if (ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " State Machine", &opened, ImGuiWindowFlags_MenuBar))
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
			ImGuiContext* context = ImGui::GetCurrentContext();

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
		ImGui::PopItemWidth();
		std::string clip_name = node->state->clip ? node->state->clip->name : "Clip";
		ImGui::Button(clip_name.c_str());
		DropAnimation(node->state);
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
					for (auto node : nodes)
					{

						if (node->input == end_pin_id)
						{
							new_transition->target_hash = node->state->name_hash;
						}

						if (node->output == start_pin_id)
						{
							new_transition->source_hash = node->state->name_hash;
						}
					}
					state_machine->transitions.push_back(new_transition);
					links.push_back(new LinkInfo{ ax::NodeEditor::LinkId(uniqueid++) , start_pin_id, end_pin_id, new_transition });

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
		auto& links = GetSelectedLinks();
		if (links.size() > 0 && ImGui::BeginMenu("Link Menu"))
		{

			ImGui::PushID(links[0]->id.AsPointer());
			ImGui::InputScalar("###Interpolation", ImGuiDataType_U64, &(links[0]->transition->interpolation_time));
			ImGui::PopID();
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


void PanelStateMachine::DropAnimation(std::shared_ptr<State> & state)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("DND_File"))
		{
			assert(payload->DataSize == sizeof(File*));
			File* incoming_file = *(File * *)payload->Data;
			if (incoming_file->file_type == FileType::ANIMATION)
			{
				state_machine->AddClipToState(state, *incoming_file);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void PanelStateMachine::LeftPanel()
{
	if (ImGui::BeginChild("Details", ImVec2(300, 0)))
	{
		if (ImGui::Button("Save"))
		{
			state_machine->Save();
		}
		if (ImGui::Button("Load"))
		{
			OpenStateMachine(state_machine->exported_file);
		}
	}
	ImGui::EndChild();
	ImGui::SameLine(0.0f, 12.0f);
}
void PanelStateMachine::OpenStateMachine(const File & file)
{
	nodes.clear();
	links.clear();
	state_machine = std::make_shared<StateMachine>(file.file_path);
	state_machine->Load(file);

	//TODO: Remove, this is only for testing
	ImportOptions options;
	Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(file), options);
	std::shared_ptr<StateMachine> test = App->resources->Load<StateMachine>(options.exported_file);

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
		for (auto & node : nodes)
		{
			if (node->state->name_hash == link->source_hash)
			{
				source = node->output;
			}
			if (node->state->name_hash == link->target_hash)
			{
				target = node->input;
			}
		}
		links.push_back(new LinkInfo{ ax::NodeEditor::LinkId(uniqueid++) , target, source, link });
	}
}
