#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_

#include "NodeEditor/imgui_node_editor.h"
#include "EditorUI/Panel/Panel.h"
#include "ResourceManagement/Resources/StateMachine.h"

struct LinkInfo
{
	ax::NodeEditor::LinkId id;
	ax::NodeEditor::PinId input_id;
	ax::NodeEditor::PinId output_id;
	std::shared_ptr<Transition> transition;
};

struct NodeInfo
{
	ax::NodeEditor::NodeId id;
	std::vector<ax::NodeEditor::PinId> inputs;
	std::vector<ax::NodeEditor::PinId> outputs;
	std::shared_ptr<State> state;
};


class File;
class PanelStateMachine :public Panel
{

public:
	PanelStateMachine();
	~PanelStateMachine();

	void Render() override;
	void HandleInteraction();
	void OpenStateMachine(const File & file);

	void RenderStates() const;
	void LeftPanel();
	void CreateNodeMenu();

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
	std::shared_ptr<StateMachine> state_machine;
	bool firstFrame = true;
	uint64_t entry_hash = std::hash<std::string>{}("Entry");
	uint64_t end_hash = std::hash<std::string>{}("End");

	//Editor
	ImVector<LinkInfo*> links;
	ImVector<NodeInfo*> nodes;
	int uniqueid = 0;
};
#endif // !_PANELSTATEMACHINE_H_


