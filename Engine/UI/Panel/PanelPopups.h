#ifndef _PANELPOPUPS_H_
#define _PANELPOPUPS_H_

#include "Panel.h"

#include <string>

class PanelPopups : public Panel
{
public:
	PanelPopups();
	~PanelPopups() = default;

	void Render() override;

private:
	void RenderAssetsLoadingPopup();
	void RenderModelSelectorPopup();

public:
	bool show_assets_loading_popup = false;
	bool show_mesh_selector_popup = false;

	bool mesh_selector_opened = true;
};

#endif //_PANELPOPUPS_H_