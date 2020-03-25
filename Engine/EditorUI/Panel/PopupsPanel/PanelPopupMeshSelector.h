#ifndef _PANELPOPUPMESHSELECTOR_H_
#define _PANELPOPUPMESHSELECTOR_H_

#include "EditorUI/Panel/Panel.h"

#include <string>
#include <vector>

class Metafile;

class PanelPopupMeshSelector : public Panel
{
public:
	PanelPopupMeshSelector();
	~PanelPopupMeshSelector() = default;

	void Render() override;

private:
	void ShowMeshIcon(Metafile* mesh_metafile);
	void ProcessMeshMouseInput(Metafile* mesh_metafile);
	void ChangeSelectedObjectMesh() const;

public:
	bool show_mesh_selector_popup = false;
	
private:
	float mesh_icon_size = 125.f;
	Metafile* selected_mesh = nullptr;
	std::vector<Metafile*> mesh_metafiles;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPMESHSELECTOR_H_