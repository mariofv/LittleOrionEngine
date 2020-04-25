#ifndef _PANELPOPUPTEXTURESELECTOR_H_
#define _PANELPOPUPTEXTURESELECTOR_H_

#include "EditorUI/Panel/Panel.h"
#include "Module/ModuleFileSystem.h"

#include <string>

class PanelPopupTextureSelector : public Panel
{
public:
	PanelPopupTextureSelector();
	~PanelPopupTextureSelector() = default;

	void Render() override;

private:
	void ShowTextureIcon(File* file);
	void ProcessTextureMouseInput(File * file);
	void ChangeSelectedBillboardTexture(std::string path) const;

public:
	bool show_texture_selector_popup = false;

private:
	float texture_icon_size = 125.f;
	File* selected_texture = nullptr;
	std::vector<std::shared_ptr<File>> texture_files;

	bool child_window_focused = false;
};

#endif //_PANELPOPUPTEXTURESELECTOR_H_