#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_
#define IMGUI_DEFINE_MATH_OPERATORS

#include "ax/Widgets.h"

#include "Component/ComponentAnimation.h"

#include "EditorUI/Panel/Panel.h"

#include "NodeEditor/imgui_node_editor.h"

#include <algorithm>
#include <array>
#include <map>
#include <string>
#include <utility>
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
	ImVec2 position, Size = {100.0f, 100.0f};
	std::shared_ptr<State> state;
	float speed = 1.0f;
	
};

class File;

class PanelStateMachine : public Panel
{

public:
	PanelStateMachine();
	~PanelStateMachine();

	void Render() override;
	bool IsElegibleStateMachine(AnimController * controller);
	void HandleInteraction();
	void InteractionDelete();
	void InteractionCreation();
	void OpenStateMachine(uint32_t state_machine_uuid);

	AnimController* GetHierarchyAnimation();

	void RenderStates();
	void LeftPanel();
	void CreateNodeMenu();

	//polishing node editor
	ImDrawList* draw_list = nullptr;

	//debug playing animation
	AnimController* animation_controller = nullptr;

private:
	std::vector<NodeInfo*> GetSelectedNodes();
	std::vector<LinkInfo*> GetSelectedLinks();

	void EraseNameFromVariables(std::vector<std::string>& names, uint64_t name_to_delete_hash);

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
	std::vector<NodeInfo*> selected_nodes;
	NodeInfo* selected_node = nullptr ;
	bool modified_by_user = false;
	std::string float_auxiliar_variable;
	std::string int_auxiliar_variable;
	std::string bool_auxiliar_variable;

	std::array<const char* , 4> comparator_name {"Greater", "Lesser", "Equal", "Not Equal"};
	std::array<const char* , 2> bool_conditions {"false", "true"};

};
#endif // !_PANELSTATEMACHINE_H_


