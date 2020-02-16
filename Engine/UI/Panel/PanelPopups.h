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
	bool assets_loading_popup_shown = false;
};

#endif //_PANELPOPUPS_H_