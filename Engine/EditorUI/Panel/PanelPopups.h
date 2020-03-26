#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
#include "PopupsPanel/PanelPopupResourceSelector.h"
#include "PopupsPanel/PanelPopupSceneManagement.h"

#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

	void CreateScript();

private:
	void RenderAssetsLoadingPopup();

public:
	PanelPopupResourceSelector resource_selector_popup;
	PanelPopupSceneManagement scene_management_popup;

	bool create_script_shown = false;
	bool show_assets_loading_popup = false;
};

#endif //_PANELPOPUPS_H_