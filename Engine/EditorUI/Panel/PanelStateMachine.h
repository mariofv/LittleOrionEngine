#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_
#define IMGUI_DEFINE_MATH_OPERATORS

#include "NodeEditor/imgui_node_editor.h"
#include "EditorUI/Panel/Panel.h"

#include "ax/Widgets.h"

#include <vector>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

//namespace ed = ax::NodeEditor; Implement in the future

using namespace ax;
using ax::Widgets::IconType;

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
	ImVec2 Pos, Size;
	//ImColor Color;
	std::shared_ptr<State> state;
	//drawing pins
	ImVec2 GetInputSlotPos(std::string target) const { return ImVec2(Pos.x, Pos.y); }
	ImVec2 GetOutputSlotPos(std::string source) const { return ImVec2(Pos.x, Pos.y); }
	//
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
	void OpenStateMachine(uint32_t state_machine_uuid);

	void RenderStates();
	void LeftPanel();
	void CreateNodeMenu();

	ImDrawList* draw_list = nullptr;
	const float NODE_SLOT_RADIUS = 6.2f;
	
private:
	std::vector<NodeInfo*> GetSelectedNodes();
	std::vector<LinkInfo*> GetSelectedLinks();

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
	std::shared_ptr<StateMachine> state_machine;
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


