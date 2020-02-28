#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
#include "PopupsPanel/PanelPopupMeshSelector.h"

#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

private:
	void RenderAssetsLoadingPopup();

public:
	bool show_assets_loading_popup;
	PanelPopupMeshSelector mesh_selector_popup;

};

#endif //_PANELPOPUPS_H_