#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"
#include "PopupsPanel/PanelPopupResourceSelector.h"
#include "PopupsPanel/PanelPopupSceneLoader.h"
#include "PopupsPanel/PanelPopupSceneSaver.h"

#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

	void RenderCreateScriptPopup();
	void RenderAddTagPopup();

private:
	void RenderAssetsLoadingPopup();

public:

	PanelPopupResourceSelector resource_selector_popup;
	PanelPopupSceneLoader scene_loader_popup;
	PanelPopupSceneSaver scene_saver_popup;

	bool create_script_popup_shown = false;
	bool add_tag_popup_shown = false;
	bool show_assets_loading_popup = false;
};

#endif //_PANELPOPUPS_H_