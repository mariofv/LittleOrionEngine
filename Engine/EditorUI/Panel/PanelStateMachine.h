#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_

#include "NodeEditor/imgui_node_editor.h"
#include "EditorUI/Panel/Panel.h"

class PanelStateMachine :public Panel
{

public:
	PanelStateMachine();
	~PanelStateMachine();

	void Render() override;

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
};
#endif // !_PANELSTATEMACHINE_H_


