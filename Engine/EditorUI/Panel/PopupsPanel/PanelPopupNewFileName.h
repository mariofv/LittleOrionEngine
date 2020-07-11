#ifndef _PANELPOPUPSNEWFILENAME_H_
#define _PANELPOPUPSNEWFILENAME_H_

#include "EditorUI/Panel/Panel.h"

#include <vector>
#include <string>

#include <imgui.h>
#include <imfilebrowser.h>

class Path;
class PanelPopupNewFileName : public Panel
{
public:
	PanelPopupNewFileName() = default;
	~PanelPopupNewFileName() = default;

	void Render() override;


public:
	bool show_new_filename_popup = false;
	std::string new_filename;
	std::function<void(std::string&)> apply_new_name;

private:
	bool pending_to_be_closed = false;
};

#endif
