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
	ax::NodeEditor::DestroyEditor(editor_context);
}

void ImGuiEx_BeginColumn()
{
	ImGui::BeginGroup();
}

void ImGuiEx_NextColumn()
{
	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();
}

void ImGuiEx_EndColumn()
{
	ImGui::EndGroup();
}


static int g_NextLinkId = 100;
void PanelStateMachine::Render()
{
	if (ImGui::Begin(ICON_FA_PROJECT_DIAGRAM " State Machine", &opened, ImGuiWindowFlags_MenuBar))
	{

			ax::NodeEditor::SetCurrentEditor(editor_context);

			LeftPanel();
			ax::NodeEditor::Begin("My Editor");
			{
				if (ImGui::IsMouseClicked(1))
				{
					ax::NodeEditor::Suspend();
					ImGui::OpenPopup("Editor Menu");
					ax::NodeEditor::Resume();
				}
				RenderStates();
				HandleInteraction();
				CreateNodeMenu();
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

void PanelStateMachine::HandleInteraction()
{
	// 2) Handle interactions
	//

	// Handle creation action, returns true if editor want to create new object (node or link)
	if (ax::NodeEditor::BeginCreate())
	{
		ax::NodeEditor::PinId inputPinId, outputPinId;
		/*if (ax::NodeEditor::QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId) // both are valid, let's accept link
			{
				// ax::NodeEditor::AcceptNewItem() return true when user release mouse button.
				if (ax::NodeEditor::AcceptNewItem())
				{
					// Since we accepted new link, lets add one to our list of links.
					g_Links.push_back({ ax::NodeEditor::LinkId(g_NextLinkId++), inputPinId, outputPinId });

					// Draw new link.
					ax::NodeEditor::Link(g_Links.back().Id, g_Links.back().InputId, g_Links.back().OutputId);
				}

				// You may choose to reject connection between these nodes
				// by calling ax::NodeEditor::RejectNewItem(). This will allow editor to give
				// visual feedback by changing link thickness and color.
			}
		}*/
		ax::NodeEditor::PinId pinId = 0;
		if (ax::NodeEditor::QueryNewNode(&pinId))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize("+ Create Node");

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + spacing.x, ImGui::GetCursorPos().y -spacing.y));

			ImVec2 cursor_screen = ImGui::GetCursorScreenPos();
			auto rectMin = ImVec2(cursor_screen.x - padding.x, cursor_screen.y- padding.y);
			auto rectMax = ImVec2(cursor_screen.x + size.x + padding.x, cursor_screen.y + size.y + padding.x);

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, ImColor(32, 45, 32, 180), size.y * 0.15f);
			ImGui::TextUnformatted("+ Create Node");

			if (ax::NodeEditor::AcceptNewItem())
			{
				ax::NodeEditor::Suspend();
				ImGui::OpenPopup("Editor Menu");
				ax::NodeEditor::Resume();
			}
		}
	}
	ax::NodeEditor::EndCreate(); // Wraps up object creation action handling.
	// Handle deletion action
	if (ax::NodeEditor::BeginDelete())
	{
		// There may be many links marked for deletion, let's loop over them.
		ax::NodeEditor::LinkId deletedLinkId;
		while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId))
		{
			// If you agree that link can be deleted, accept deletion.
			if (ax::NodeEditor::AcceptDeletedItem())
			{
				// Then remove link from your data.
				for (auto& link : links)
				{
					/*if (link.Id == deletedLinkId)
					{
						g_Links.erase(&link);
						break;
					}*/
				}
			}

		}
	}
	ax::NodeEditor::EndDelete(); // Wrap up deletion action
}


void PanelStateMachine::OpenStateMachine(const File & file)
{
	state_machine = std::make_shared<StateMachine>(file.file_path);
	state_machine->Load(file);

	//TODO: Remove, this is only for testing
	ImportOptions options;
	Importer::GetOptionsFromMeta(Importer::GetMetaFilePath(file), options);
	std::shared_ptr<StateMachine> test = App->resources->Load<StateMachine>(options.exported_file);
}

void PanelStateMachine::RenderStates() const
{
	int uniqueId = 5;
	ImVec2 position(0,0);
	for (auto & state : state_machine->states)
	{
		// Start drawing nodes.
		int node = uniqueId++;
		if (firstFrame)
			ax::NodeEditor::SetNodePosition(node, position);
		ax::NodeEditor::BeginNode(node);
		ImGui::Text(state->name.c_str());
		ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
		ImGui::Text("Out ->");
		ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();
		position = ax::NodeEditor::GetNodeSize(node);
		position.y = 0;
		position.x += 10;

	}
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
				state_machine->states.push_back(std::make_shared<State>("New Node " + new_states, nullptr));
				new_states++;
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