#ifndef _PANELSTATEMACHINE_H_
#define _PANELSTATEMACHINE_H_

#include "NodeEditor/imgui_node_editor.h"
#include "EditorUI/Panel/Panel.h"
#include "ResourceManagement/Resources/StateMachine.h"
class File;
class PanelStateMachine :public Panel
{

public:
	PanelStateMachine();
	~PanelStateMachine();

	void Render() override;
	void OpenStateMachine(const File & file);

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
	std::shared_ptr<StateMachine> state_machine;
};
#endif // !_PANELSTATEMACHINE_H_


