#ifndef _PANELPOPUPSCENESAVER_H_
#define _PANELPOPUPSCENESAVER_H_

#include "EditorUI/Panel/Panel.h"

#include <vector>
#include <string>

#include <imgui.h>
#include <imfilebrowser.h>

class Path;

class PanelPopupSceneSaver : public Panel
{
public:
	PanelPopupSceneSaver();
	~PanelPopupSceneSaver() = default;

	void Render() override;

	bool HasSelected() const;
	std::string GetSelected();

private:
	void RenderAccessPath();
	void RenderCurrentFolderContents();

	void PushCurrentPath(Path* new_current_path);
	void PopCurrentPath(Path* new_current_path);

	std::string GetNormalizedPath();
	void SetPopupSelection();

	void ConfirmationPopup();

	float GetFrameHeightWithSpacing();

public:
	bool popup_shown = false;

private:
	std::vector<Path*> path_stack;
	Path* current_path = nullptr;

	int selected_path = -1;
	std::string selected_file_name;

	bool pending_to_be_closed = false;
	bool has_selected = false;
	bool is_overwriting = false;
};

#endif //_PANELPOPUPSCENESAVER_H_