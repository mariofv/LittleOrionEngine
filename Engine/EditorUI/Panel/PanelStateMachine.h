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
	void HandleInteraction();
	void OpenStateMachine(const File & file);

	void RenderStates() const;
	void CreationIterations(); 
	void CreateNodeMenu();

private:
	ax::NodeEditor::EditorContext* editor_context = nullptr;
	std::shared_ptr<StateMachine> state_machine;
	bool firstFrame = true;
};
#endif // !_PANELSTATEMACHINE_H_


