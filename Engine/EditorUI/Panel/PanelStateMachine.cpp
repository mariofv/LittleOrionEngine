#include "PanelStateMachine.h"
#include "Filesystem/File.h"
#include <FontAwesome5/IconsFontAwesome5.h>

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"

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
	links.erase(links.begin(), links.end());
	ax::NodeEditor::DestroyEditor(editor_context);
}

void PanelStateMachine::Render()
{
	if (ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " State Machine", &opened, ImGuiWindowFlags_MenuBar))
	{

			ax::NodeEditor::SetCurrentEditor(editor_context);

			LeftPanel();
			ax::NodeEditor::Begin("My Editor");
			{
				/*if (ImGui::IsMouseClicked(1))
				{
					ax::NodeEditor::Suspend();
					ImGui::OpenPopup("Editor Menu");
					ax::NodeEditor::Resume();
				}*/
				RenderStates();
				//HandleInteraction();
				//CreateNodeMenu();
				ax::NodeEditor::End();
				if (firstFrame)
				{
					ax::NodeEditor::NavigateToContent(0.0f);
				}
				firstFrame = false;
			}

	}
	ImGui::End();
}
void PanelStateMachine::RenderStates() const
{
	int uniqueId = 5;
	ImVec2 position(0,0);
	for (auto & node : nodes)
	{
		// Start drawing nodes.
		if (firstFrame)
		{
			ax::NodeEditor::SetNodePosition(node->id, position);
		}
		ax::NodeEditor::BeginNode(node->id);
		ImGui::Text(node->state->name.c_str());
		ImGui::Button("Clip");
		for (auto & output :  node->outputs)
		{
			ax::NodeEditor::BeginPin(output, ax::NodeEditor::PinKind::Output);
			ImGui::Text("Out ->");
			ax::NodeEditor::EndPin();
		}
		for (auto & output : node->outputs)
		{
			ax::NodeEditor::BeginPin(output, ax::NodeEditor::PinKind::Input);
			ImGui::Text("-> In");
			ax::NodeEditor::EndPin();
		}
		ax::NodeEditor::EndNode();

	}
	for (auto& linkInfo : links)
	{
		ax::NodeEditor::Link(linkInfo->id, linkInfo->input_id, linkInfo->output_id);
	}
}

void PanelStateMachine::HandleInteraction()
{
	
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
				node->inputs.push_back(uniqueid++);
				node->outputs.push_back(uniqueid++);
				nodes.push_back(std::move(node));
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	ax::NodeEditor::Resume();
}


void PanelStateMachine::LeftPanel()
{
	if (ImGui::BeginChild("Details", ImVec2(300, 0)))
	{
	}
	ImGui::EndChild();
	ImGui::SameLine(0.0f, 12.0f);
}
void PanelStateMachine::OpenStateMachine(const File & file)
{
	state_machine = std::make_shared<StateMachine>(file.file_path);
	state_machine->Load(file);

	//TODO: Remove, this is only for testing
	ImportOptions options;
	Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(file), options);
	std::shared_ptr<StateMachine> test = App->resources->Load<StateMachine>(options.exported_file);

	//Tranform form state machine to ui
	for (auto & link : state_machine->transitions)
	{
		links.push_back(new LinkInfo{ ax::NodeEditor::LinkId(uniqueid++) ,  ax::NodeEditor::PinId(uniqueid++),  ax::NodeEditor::PinId(uniqueid++), link});
	}
	for (auto & state : state_machine->states)
	{
		NodeInfo * node = new NodeInfo({ ax::NodeEditor::NodeId(uniqueid++) });
		node->state = state;
		for (auto & link : links)
		{
			if (link->transition->source_hash == state->name_hash)
			{
				node->outputs.push_back(link->output_id);
			}
			if (link->transition->target_hash == state->name_hash)
			{
				node->outputs.push_back(link->input_id);
			}
		}
		nodes.push_back(node);
	}
}