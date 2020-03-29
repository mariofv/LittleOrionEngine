#include "PanelStateMachine.h"
#include "Filesystem/File.h"
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
	ax::NodeEditor::DestroyEditor(editor_context);
}

void PanelStateMachine::Render()
{
	if (ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " State Machine", &opened, ImGuiWindowFlags_MenuBar))
	{
		ax::NodeEditor::SetCurrentEditor(editor_context);

		ax::NodeEditor::Begin("My Editor");

		int uniqueId = 1;

		// Start drawing nodes.
		ax::NodeEditor::BeginNode(uniqueId++);
		ImGui::Text("Node A");
		ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
		ImGui::Text("-> In");
		ax::NodeEditor::EndPin();
		ImGui::SameLine();
		ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();

		ax::NodeEditor::End();
	}
	ImGui::End();
}

void PanelStateMachine::OpenStateMachine(const File & file)
{
	state_machine = std::make_shared<StateMachine>(file.file_path);
	state_machine->Load(file);
}
