#ifndef _PANELPOPUPRESOURCESELECTOR_H_
#define _PANELPOPUPRESOURCESELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "ResourceManagement/Metafile/Metafile.h"

#include <string>

class PanelPopupResourceSelector : public Panel
{
public:
	PanelPopupResourceSelector();
	~PanelPopupResourceSelector() = default;

	void Render() override;

	void ShowPanel(ResourceType resource_type);

private:
	void ShowResourceIcon(Metafile* file);
	void ProcessMouseInput(Metafile * file);
	void ChangeSelectedObjectResource() const;
	std::string GetResourceName() const;
	
private:
	ResourceType resource_type = ResourceType::UNKNOWN;
	std::string resource_name = "";

	bool show_resource_selector_popup = false;
	float resource_icon_size = 125.f;

	Metafile* selected_resource_metafile = nullptr;
	std::vector<Metafile*> resource_metafiles;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPRESOURCESELECTOR_H_