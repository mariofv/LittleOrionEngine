#ifndef _PANELPOPUPMESHSELECTOR_H_
#define _PANELPOPUPMESHSELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

#include <string>

class PanelPopupMeshSelector : public Panel
{
public:
	PanelPopupMeshSelector();
	~PanelPopupMeshSelector() = default;

	void Render() override;

private:
	void ShowMeshIcon(Path* file);
	void ProcessMeshMouseInput(Path * file);
	void ChangeSelectedObjectMesh() const;

public:
	bool show_mesh_selector_popup = false;
	
private:
	float mesh_icon_size = 125.f;
	Path* selected_mesh = nullptr;
	std::vector<std::shared_ptr<Path>> mesh_files;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPMESHSELECTOR_H_