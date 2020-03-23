#ifndef _PANELPOPUPMATERIALSELECTOR_H_
#define _PANELPOPUPMATERIALSELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

#include <string>

class PanelPopupMaterialSelector : public Panel
{
public:
	PanelPopupMaterialSelector();
	~PanelPopupMaterialSelector() = default;

	void Render() override;

private:
	void ShowMaterialIcon(Path* file);
	void ProcessMaterialMouseInput(Path * file);
	void ChangeSelectedObjectMaterial() const;

public:
	bool show_material_selector_popup = false;
	
private:
	float material_icon_size = 125.f;
	Path* selected_material = nullptr;
	std::vector<std::shared_ptr<Path>> material_files;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPMATERIALSELECTOR_H_