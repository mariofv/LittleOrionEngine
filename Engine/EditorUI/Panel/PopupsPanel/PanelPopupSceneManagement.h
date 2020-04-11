#ifndef _PANELPOPUPSCENEMANAGEMENT_H_
#define _PANELPOPUPSCENEMANAGEMENT_H_

#include "EditorUI/Panel/Panel.h"

#include <vector>
#include <string>

#include <imgui.h>
#include <imfilebrowser.h>

class Path;

class PanelPopupSceneManagement : public Panel
{
public:
	PanelPopupSceneManagement();
	~PanelPopupSceneManagement() = default;

	void Render() override;

	bool HasSelected() const;
	std::string GetSelected() const;

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

	bool has_selected = false;
};

#endif //_PANELPOPUPSCENEMANAGEMENT_H_