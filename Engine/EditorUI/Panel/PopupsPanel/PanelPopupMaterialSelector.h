#ifndef _PANELPOPUPMATERIALSELECTOR_H_
#define _PANELPOPUPMATERIALSELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "ResourceManagement/Metafile/Metafile.h"

#include <string>

class PanelPopupMaterialSelector : public Panel
{
public:
	PanelPopupMaterialSelector();
	~PanelPopupMaterialSelector() = default;

	void Render() override;

private:
	void ShowMaterialIcon(Metafile* file);
	void ProcessMaterialMouseInput(Metafile * file);
	void ChangeSelectedObjectMaterial() const;

public:
	bool show_material_selector_popup = false;
	
private:
	float material_icon_size = 125.f;
	Metafile* selected_material = nullptr;
	std::vector<Metafile*> material_metafiles;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPMATERIALSELECTOR_H_