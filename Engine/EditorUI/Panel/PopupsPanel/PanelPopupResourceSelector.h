#ifndef _PANELPOPUPRESOURCESELECTOR_H_
#define _PANELPOPUPRESOURCESELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "ResourceManagement/Metafile/Metafile.h"

#include <string>

class Resource;

class PanelPopupResourceSelector : public Panel
{
public:
	PanelPopupResourceSelector();
	~PanelPopupResourceSelector() = default;

	void Render() override;

	void ShowPanel(unsigned int element_id, ResourceType resource_type);
	void GetSelectedResource(unsigned int element_id, std::shared_ptr<Resource>& return_value);
private:
	void ClosePanel();

	void ShowResourceIcon(Metafile* file);
	void SelectResource();

	void ProcessMouseInput(Metafile * file);
	std::string GetResourceName() const;
	
private:
	unsigned int element_id = 0;
	
	ResourceType resource_type = ResourceType::UNKNOWN;
	std::string resource_name = "";

	std::shared_ptr<Resource> selected_resource = nullptr;
	Metafile* selected_resource_metafile = nullptr;
	std::vector<Metafile*> resource_metafiles;

	bool show_resource_selector_popup = false;
	float resource_icon_size = 125.f;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPRESOURCESELECTOR_H_