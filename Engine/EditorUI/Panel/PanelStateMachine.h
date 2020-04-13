#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_

#include "NodeEditor/imgui_node_editor.h"
#include "EditorUI/Panel/Panel.h"

#include <vector>
class StateMachine;
struct Transition;
struct State;

struct LinkInfo
{
	ax::NodeEditor::LinkId id;
	ax::NodeEditor::PinId input_id;
	ax::NodeEditor::PinId output_id;
	std::string target;
	std::string source;
	std::shared_ptr<Transition> transition;
};

struct NodeInfo
{
	ax::NodeEditor::NodeId id;
	ax::NodeEditor::PinId input;
	ax::NodeEditor::PinId output;
	std::shared_ptr<State> state;
};


class File;
class PanelStateMachine : public Panel
{

public:
	PanelStateMachine();
	~PanelStateMachine();

	void Render() override;
	void HandleInteraction();
	void InteractionDelete();
	void InteractionCreation();
	void OpenStateMachine(const File & file);

	void RenderStates();
	void LeftPanel();
	void CreateNodeMenu();

private:
	void DropAnimation(std::shared_ptr<State>& state);
	std::vector<NodeInfo*> GetSelectedNodes();

	std::vector<LinkInfo*> GetSelectedLinks();

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
	std::shared_ptr<StateMachine> state_machine;
	std::shared_ptr<StateMachine> state_machine_cache;
	bool firstFrame = true;
	uint64_t entry_hash = std::hash<std::string>{}("Entry");
	uint64_t end_hash = std::hash<std::string>{}("End");

	//Editor
	ImVector<LinkInfo*> links;
	ImVector<NodeInfo*> nodes;
	int uniqueid = 1;
	std::shared_ptr<State> selected_state;
};
#endif // !_PANELSTATEMACHINE_H_


